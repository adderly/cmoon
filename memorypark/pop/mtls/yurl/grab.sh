#!/bin/sh

for file in *.html; do
    echo "Process ${file}..."
    ID=`echo ${file} | egrep -o "[0-9]+"`
    NAME=`grep -i -o -P "<H1>.*</H1>" ${file} | sed 's/<[^>]*>//g'`
    CITY=`grep -o "中国黄页网</a> >> <a href=.*>.*</a> >>" ${file} | grep -o 'html>[^<]*' | sed 's/html>//g' | sed 's/黄页//g'`
    IND=`grep -o "行业:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/行业:<\/B>//g' | sed 's/<BR>//g'`
    ADDR=`grep -o "地址:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/地址:<\/B>//g' | sed 's/<BR>//g'`
    URL=`grep -o "网址:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/网址:<\/B>//g' | sed 's/<BR>//g'`
    BODY=`grep -o "联系人:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/联系人:<\/B>//g' | sed 's/<BR>//g'`
    TEL=`grep -o "电话:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/电话:<\/B>//g' | sed 's/<BR>//g'`
    FAX=`grep -o "传真:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/传真:<\/B>//g' | sed 's/<BR>//g'`
    ZIP=`grep -o "邮编:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/邮编:<\/B>//g' | sed 's/<BR>//g'`
    EMAIL=`grep -o "电子邮件:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/电子邮件:<\/B>//g' | sed 's/<BR>//g'`
    QQ=`grep -o "QQ:</B>[^<\\'\"\;]*<BR>" ${file} | sed 's/QQ:<\/B>//g' | sed 's/<BR>//g'`
    PRDINTR=`grep -o "产品介绍:</B>[^<\\'\"\;]*<" ${file} | sed 's/产品介绍:<\/B>//g' | sed 's/<//g'`
    INTRO=`grep -o "单位介绍:</B>[^<\\'\"\;]*<" ${file} | sed 's/单位介绍:<\/B>//g' | sed 's/<//g'`

    echo "INSERT INTO com (id, name, city, industry, addr, url, body, tel, fax, zipcode, email, qq, prod, intro) VALUES ($ID, '${NAME}'::varchar(256), '${CITY}'::varchar(64), '${IND}'::varchar(256), '${ADDR}'::varchar(256), '${URL}'::varchar(256), '${BODY}'::varchar(64), '${TEL}'::varchar(64), '${FAX}'::varchar(64), '${ZIP}'::varchar(64), '${EMAIL}'::varchar(64), '${QQ}'::varchar(64), '${PRDINTR}', '${INTRO}');" >> com.sql
done
