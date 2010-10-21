#ifndef __LTPL_H__
#define __LTPL_H__
#include "mheads.h"

__BEGIN_DECLS

void ltpl_prepare_rend(HDF *hdf, char *tpl);
int ltpl_parse_file(HASH *dbh, void *lib, char *dir, char *name, HASH *outhash);
int  ltpl_parse_dir(char *dir, HASH *outhash);
int  ltpl_init(HASH **tplh);
void ltpl_destroy(HASH *tplh);
int  ltpl_render(CGI *cgi, HASH *tplh, session_t *ses);

__END_DECLS
#endif	/* __LTPL_H__ */
