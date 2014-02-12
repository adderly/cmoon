#!/bin/sh

/usr/local/moon/pop/mtls/prestat

psql -U lcser lcs_mtool -c "INSERT INTO visit (aid, pv, uv) SELECT * FROM visit_tohour;"
psql -U lcser lcs_mtool -c "INSERT INTO topref (aid, refer, count) SELECT * FROM topref_tohour;"
psql -U lcser lcs_mtool -c "INSERT INTO topurl (aid, url, title, count) SELECT * FROM topurl_tohour;"
psql -U lcser lcs_mtool -c "INSERT INTO toparea (aid, area, count) SELECT * FROM toparea_tohour;"

/usr/local/moon/pop/mtls/clearcache mtls
