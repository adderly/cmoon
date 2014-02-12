#!/bin/sh

CUR_DATE=`date +"%F"`
BACKUP_DIR=/data/dbs/

pg_dump -U lcser lcs_main > ${BACKUP_DIR}${CUR_DATE}.lcs_main
pg_dump -U lcser lcs_dyn > ${BACKUP_DIR}${CUR_DATE}.lcs_dyn
pg_dump -U lcser lcs_msg > ${BACKUP_DIR}${CUR_DATE}.lcs_msg
pg_dump -U lcser lcs_mtool > ${BACKUP_DIR}${CUR_DATE}.lcs_mtool
pg_dump -U lcser lcs_aux > ${BACKUP_DIR}${CUR_DATE}.lcs_aux
