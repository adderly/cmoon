DROP VIEW visit_tohour;
DROP VIEW topref_tohour;
DROP VIEW topurl_tohour;
DROP VIEW toparea_tohour;

CREATE VIEW visit_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, COUNT(id), COUNT(DISTINCT uid) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid') AS t1(aid int, pv int, uv int);
CREATE VIEW topref_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, refer, COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, refer') AS t1(aid int, refer varchar(256), count int);
CREATE VIEW topurl_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, url, max(title), COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, url') AS t1(aid int, url varchar(256), title varchar(256), count int);
CREATE VIEW toparea_tohour AS SELECT * FROM dblink('dbname=lcs_dyn host=localhost user=lcser password=loveu', 'SELECT aid, area, COUNT(id) FROM track WHERE intime > now() - interval ''1 hour'' GROUP BY aid, area') AS t1(aid int, area varchar(256), count int);

\c lcs_mtool;

alter table visit ALTER dt TYPE timestamp;
alter table topref ALTER dt TYPE timestamp;
alter table topurl ALTER dt TYPE timestamp;
alter table toparea ALTER dt TYPE timestamp;

alter table visit ALTER dt set default now();
alter table topref ALTER dt set default now();
alter table topurl ALTER dt set default now();
alter table toparea ALTER dt set default now();
