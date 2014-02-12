#ifndef __LERR_H__
#define __LERR_H__
#include "mheads.h"

__BEGIN_DECLS

extern NERR_TYPE LERR_NOTLOGIN;

extern NERR_TYPE LERR_NREGIST;            
extern NERR_TYPE LERR_ALREADYREGIST;        
extern NERR_TYPE LERR_MISSEMAIL;            
extern NERR_TYPE LERR_NRESET;
extern NERR_TYPE LERR_WRESET;
extern NERR_TYPE LERR_NOTJOIN;
extern NERR_TYPE LERR_ALREADYJOIN;

extern NERR_TYPE LERR_NCHARGE;
extern NERR_TYPE LERR_NEEDUP;
extern NERR_TYPE LERR_NEEDWT;
extern NERR_TYPE LERR_NEED_ADMIN;            
extern NERR_TYPE LERR_NEED_ROOT;

extern NERR_TYPE LERR_LOGINPSW;
extern NERR_TYPE LERR_USERINPUT;
extern NERR_TYPE LERR_LIMIT;                
extern NERR_TYPE LERR_MISS_DATA;
extern NERR_TYPE LERR_MISS_TPL;            
extern NERR_TYPE LERR_IMGPROE;            
extern NERR_TYPE LERR_ATTACK;            

NEOERR* lerr_init();
void lerr_opfinish_json(NEOERR *err, HDF *hdf);

__END_DECLS
#endif    /* __LERR_H__ */
