#include "mheads.h"
#include "lheads.h"

/* LERR_XXX start from 25 */

/*
 * static error (layout.html used)
 */
int LERR_NOTLOGIN = 0;            /* 25 */

/*
 * mevent plugin error
 */
/* aic */
int LERR_NREGIST = 0;            /* 26 */
int LERR_ALREADYREGIST = 0;        /* 27 */
int LERR_MISSEMAIL = 0;            /* 28 */
int LERR_NRESET = 0;            /* 29 */
int LERR_WRESET = 0;            /* 30 */
int LERR_NOTJOIN = 0;            /* 31 */
int LERR_ALREADYJOIN = 0;        /* 32 */

/* bank */
int LERR_NCHARGE = 0;            /* 33 */
int LERR_NEEDUP = 0;            /* 34 */
int LERR_NEEDWT = 0;            /* 35 */
int LERR_NEED_ADMIN = 0;        /* 36 */
int LERR_NEED_ROOT = 0;            /* 37 */

/*
 * app error
 */
int LERR_LOGINPSW = 0;
int LERR_USERINPUT = 0;
int LERR_LIMIT = 0;
int LERR_MISS_DATA = 0;
int LERR_MISS_TPL = 0;
int LERR_IMGPROE = 0;
int LERR_ATTACK = 0;


static int lerrInited = 0;

NEOERR* lerr_init()
{
    NEOERR *err;

    if (lerrInited == 0) {
        err = nerr_init();
        if (err != STATUS_OK) return nerr_pass(err);

        err = merr_init((MeventLog)mtc_msg);
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_NOTLOGIN, "请登录后操作");
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_NREGIST, "站点不存在");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_ALREADYREGIST, "站点名已被注册");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MISSEMAIL, "注册时没提供邮箱");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NRESET, "请先申请密码重置");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_WRESET, "邮箱验证码错误");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NOTJOIN, "用户还没来访");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_ALREADYJOIN, "用户已在列表中");
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_NCHARGE, "请先站内充值！");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NEEDUP, "站长帐号余额不足，请先 "
                            " <a href='/app/charge'>充值！</a>");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NEEDWT, "充值还未确认，请稍作等待。");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NEED_ADMIN, "需要管理员权限");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_NEED_ROOT, "让 kol 来试试吧！");
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_LOGINPSW, "密码错误");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_USERINPUT, "输入参数错误");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_LIMIT, "用户无权限");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MISS_DATA, "资源不存在");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MISS_TPL, "找不到渲染模板(忘记了/json ?)");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_IMGPROE, "处理图片失败");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_ATTACK, "太过频繁，请稍后请求！");
        if (err != STATUS_OK) return nerr_pass(err);


        lerrInited = 1;
    }

    return STATUS_OK;
}

void lerr_opfinish_json(NEOERR *err, HDF *hdf)
{
    if (err == STATUS_OK) {
        hdf_set_value(hdf, PRE_SUCCESS, "1");
        return;
    }
    
    hdf_remove_tree(hdf, PRE_SUCCESS);
    
    char buf[1024];
    NEOERR *neede = mcs_err_valid(err);
    if (!neede) neede = err;
    /* set PRE_ERRXXX with the most recently err */
    if (!hdf_get_obj(hdf, PRE_ERRMSG)) {
        hdf_set_value(hdf, PRE_ERRMSG, _lookup_errname(neede, buf, sizeof(buf)));
    }
    hdf_set_int_value(hdf, PRE_ERRCODE, neede->error);

    STRING str; string_init(&str);
    nerr_error_traceback(err, &str);
    mtc_err("%s", str.buf);
    hdf_set_value(hdf, PRE_ERRTRACE, str.buf);
    nerr_ignore(&err);
    string_clear(&str);
}
