/*
 * ��ʱ���ݱ��ݹ���
 * ==============
 *
 * ��;��
 * ----
 * ������qdbm����������д��mysql���ݿ⣬����nmdb�������û�и��µġ���ʱ��û���ʵ�����
 *
 * �÷���
 * ----
 * ��Ϊqdbm����ͬʱ��������̴򿪣�������Ҫ�ȿ���һ�����µ�qdbm�ļ���Ϊ���������Դ������
 * ִ�г���ʱ��Ҫָ��qdbm���ݿ���ļ������͸��ļ���Ӧ����(domain, ��n_blog)�������Ӷ�Ӧ��nmdb
 * ���巽����ο�ִ��./storedb���������
 *
 * ԭ��
 * ----
 * ������ȡԴqdbm���ݿ��е�����key, ��nmdb��ȡ��key��Ӧ��ֵ�������²�����
 * 1, ���ݸ�key�ĸ�������increment���жϸ�����¼�Ƿ��б仯�������Ҫ�Ļ�����ֵд��mysql���ݿ�
 * 2, ���ݸ�key�ĸ�������timestamp���жϸü�¼��������ʵ�ʱ���Ƿ���һ����֮�࣬
 *    �������һ���£�����key, key_increment, key_timestamp��nmdb��ɾ�����Ա���nmdb�ĸ�Ч�ʡ�
 *    ��increment��û��д��mysql��key����ɾ����
 */
#include "mheads.h"
#include "lheads.h"
#include "ocds.h"

void useage(char *prg)
{
	printf("usage: %s -f qdbm filename -d domain name\n"
		   "\t -f the qdbm database file name need to be stored.\n"
		   "\t -d domain the db stored for.\n"
		   "example:\n"
		   "\t %s -f /data/qdbm/backup/blog -d n_blog\n",
		   prg, prg);
	exit (1);
}

char g_dbfn[LEN_FN];
char g_domain[LEN_FN];

HDF *g_cfg = NULL;

int main(int argc, char *argv[])
{
	DEPOT *qdb = NULL;
	fdb_t *fdb = NULL;
	nmdb_t *nmdb = NULL;
	
	//char *key, *val, *v;
	char *key, *v;
	char val[LEN_NMDB_VAL];
	int increment;
	int lenkey, lenval;
	bool dboffered, dmoffered;
	size_t r;
	int ret;

	mtc_init(HF_LOG_PATH"mtools.storedb");
	if (!mconfig_parse_file(CONFIG_FILE, &g_cfg)) {
		mtc_err("init config %s error", CONFIG_FILE);
		return 1;
	}

	dboffered = dmoffered = false;
	if (argc > 1) {
		int c;
		while ( (c=getopt(argc, argv, "f:d:")) != -1 ) {
			switch (c) {
			case 'f':
				dboffered = true;
				strncpy(g_dbfn, optarg, sizeof(g_dbfn));
				break;
			case 'd':
				dmoffered = true;
				strncpy(g_domain, optarg, sizeof(g_domain));
				break;
			default:
				useage(argv[0]);
			}
		}
	} else {
		useage(argv[0]);
	}
	if (!dboffered || !dmoffered) {
		useage(argv[0]);
	}

	ret = ldb_init(&fdb, NULL, NULL);
	if (ret != RET_DBOP_OK) {
		mtc_err("init db error");
		return 1;
	}
	
	nmdb = nmdb_init();
	if (nmdb == NULL) {
		mtc_err("init nmdb error");
		return 1;
	}
	ret = cds_add_udp_server(nmdb, g_domain);
	if (ret != RET_DBOP_OK) {
		mtc_err("add nmdb server for %s failure", g_domain);
		return 1;
	}

	bool repaired = false;
 reopen:
	if(!(qdb = dpopen(g_dbfn, DP_OREADER|DP_OWRITER, -1))) {
		if (!repaired) {
			mtc_warn("try to reapir %s", g_dbfn);
			dprepair(g_dbfn);
			repaired = true;
			goto reopen;
		}
		mtc_err("open %s failure %s", g_dbfn, dperrmsg(dpecode));
		return 1;
	}
	dpiterinit(qdb);
	
	char tkey[LEN_NMDB_KEY];
	time_t timenow = time(NULL);
	while ((key = dpiternext(qdb, &lenkey)) != NULL) {
		if (futil_is_userdata_key(key)) {
			snprintf(tkey, sizeof(tkey), "%s_"POST_INCREMENT, key);
			r = nmdb_get(nmdb, (unsigned char*)tkey, strlen(tkey), (unsigned char*)val, LEN_NMDB_VAL);
			if ((int)r <= 0) {
				mtc_warn("get %s failure", tkey);
				goto doclean;
			}
			*(val+r) = '\0';
			v = neos_strip(val);
			increment = atoi(v);
			/* ��increment > ��ֵ��keyд��mysql��������key_POST_INCREMENT 0 */
			if (increment > 10) {
				r = nmdb_get(nmdb, (unsigned char*)key, strlen(key), (unsigned char*)val, LEN_NMDB_VAL);
				if ((int)r <= 0) {
					mtc_err("%s increment found, but data not found, cleanup...", key);
					goto doclean;
				}
				*(val+r) = '\0';
				v = neos_strip(val);
				ret = cds_store_increment(fdb, key, v);
				if (ret != RET_DBOP_OK) {
					mtc_err("store %s for %s to db failure", v, key);
					free(key);
					continue;
				}
				strcpy(val, "0");
				r =  nmdb_set(nmdb, (unsigned char*)tkey, strlen(tkey), (unsigned char*)val, strlen(val)+1);
			} else if (increment > 0) {
				/*
				 * ������������û����ֵ��key��ɾ��
				 * ����key��Զ�����nmdb�������������
				 */
				free(key);
				continue;
			}

		doclean:
			/* ɾ�����һ��ǰ���ж�ȡ��key */
			snprintf(tkey, sizeof(tkey), "%s_"POST_TIMESTAMP, key);
			r = nmdb_get(nmdb, (unsigned char*)tkey, strlen(tkey), (unsigned char*)val, LEN_NMDB_VAL);
			if ((int)r <= 0) {
				mtc_warn("get %s failure", tkey);
				free(key);
				continue;
			}
			*(val+r) = '\0';
			/* time_t defined in bits/types.h #define __SLONGWORD_TYPE        long int */
			if (atol(val) <= timenow-(60*60*24*30)) {
				nmdb_del(nmdb, (unsigned char*)key, lenkey);
				nmdb_del(nmdb, (unsigned char*)tkey, strlen(tkey));
				snprintf(tkey, sizeof(tkey), "%s_"POST_INCREMENT, key);
				nmdb_del(nmdb, (unsigned char*)tkey, strlen(tkey));
			}
		}
		free(key);
	}
	
	dpclose(qdb);
	fdb_free(&fdb);
	if (nmdb != NULL)
		nmdb_free(nmdb);
	
	return 0;
}
