#!/bin/sh

OUTDIR=../aaaU/

for file in *.html; do
    echo "Process ${file}..."
    iconv -f gb2312 -t utf8 ${file} > ${OUTDIR}${file}
done
