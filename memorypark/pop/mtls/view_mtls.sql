DROP VIEW count_app_today;
DROP VIEW count_user_today;
DROP VIEW count_join_today;
DROP VIEW count_visit_today;
DROP VIEW count_msg_today;
DROP VIEW count_app_valid_today;

CREATE VIEW count_app_today AS SELECT * FROM dblink('dbname=lcs_main host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM appinfo WHERE intime > current_date') AS t1(cc int);
CREATE VIEW count_user_today AS SELECT * FROM dblink('dbname=lcs_main host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM userinfo WHERE intime > current_date') AS t1(cc int);
CREATE VIEW count_join_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM track WHERE intime > current_date AND type=0') AS t1(cc int);
CREATE VIEW count_visit_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM track WHERE intime > current_date AND type=1') AS t1(cc int);
CREATE VIEW count_msg_today AS SELECT * FROM dblink('dbname=lcs_msg host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM msg WHERE intime > current_date') AS t1(cc int);
CREATE VIEW count_app_valid_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(DISTINCT aname) FROM track WHERE intime > current_date AND type=0') AS t1(cc int);


DROP VIEW count_app_total;
DROP VIEW count_user_total;
DROP VIEW count_join_total;
DROP VIEW count_visit_total;
DROP VIEW count_msg_total;
DROP VIEW count_app_valid_total;

CREATE VIEW count_app_total AS SELECT * FROM dblink('dbname=lcs_main host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM appinfo') AS t1(cc int);
CREATE VIEW count_user_total AS SELECT * FROM dblink('dbname=lcs_main host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM userinfo') AS t1(cc int);
CREATE VIEW count_join_total AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM track WHERE type=0') AS t1(cc int);
CREATE VIEW count_visit_total AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM track WHERE type=1') AS t1(cc int);
CREATE VIEW count_msg_total AS SELECT * FROM dblink('dbname=lcs_msg host=localhost user=lcser password=loveu', 'SELECT COUNT(*) FROM msg') AS t1(cc int);
CREATE VIEW count_app_valid_total AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT COUNT(DISTINCT aname) FROM track WHERE type=0') AS t1(cc int);


DROP VIEW visit_today;
DROP VIEW topref_today;
DROP VIEW topurl_today;
DROP VIEW toparea_today;

CREATE VIEW visit_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, COUNT(id), COUNT(DISTINCT uid) FROM track WHERE intime > current_date GROUP BY aid') AS t1(aid int, pv int, uv int);
CREATE VIEW topref_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, refer, COUNT(id) FROM track WHERE intime > current_date GROUP BY aid, refer') AS t1(aid int, refer varchar(256), count int);
CREATE VIEW topurl_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, url, max(title), COUNT(id) FROM track WHERE intime > current_date GROUP BY aid, url') AS t1(aid int, url varchar(256), title varchar(256), count int);
CREATE VIEW toparea_today AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, area, COUNT(id) FROM track WHERE intime > current_date GROUP BY aid, area') AS t1(aid int, area varchar(256), count int);


DROP VIEW visit_tohour;
DROP VIEW topref_tohour;
DROP VIEW topurl_tohour;
DROP VIEW toparea_tohour;

CREATE VIEW visit_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, COUNT(id), COUNT(DISTINCT uid) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid') AS t1(aid int, pv int, uv int);
CREATE VIEW topref_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, refer, COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, refer') AS t1(aid int, refer varchar(256), count int);
CREATE VIEW topurl_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, url, max(title), COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, url') AS t1(aid int, url varchar(256), title varchar(256), count int);
CREATE VIEW toparea_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, area, COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, area') AS t1(aid int, area varchar(256), count int);
