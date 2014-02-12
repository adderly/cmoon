05-16
lcs_dyn:
alter table lcsjoin drop unamea;
alter table lcsjoin add oid int not null default 0;
alter table lcsjoin add oname varchar(256) not null default '';

lcs_main
alter table appinfo add pid int not null default 0;

CREATE TABLE userinfo (
       uid int,
       uname varchar(256) NOT NULL DEFAULT '',
       aid int,
       aname varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now(),
       PRIMARY KEY (uid, aid)
);

CREATE INDEX user_index ON userinfo (aid);

CREATE TRIGGER tg_uptime_user BEFORE UPDATE ON userinfo FOR EACH ROW EXECUTE PROCEDURE update_time();


05-17
lcs_main
alter table userinfo add ip varchar(64) not null default '';

12-22
======
\c lcs_dyn;

DROP TABLE lcsjoin;
DROP TABLE visit;

CREATE TABLE track (
       id SERIAL,
       type int NOT NULL DEFAULT 0, -- 0: join 1: visit
       aname varchar(256) NOT NULL DEFAULT '',
       uname varchar(256) NOT NULL DEFAULT '',
       oname varchar(256) NOT NULL DEFAULT '',
       aid int NOT NULL DEFAULT 0,
       uid int NOT NULL DEFAULT 0,
       oid int NOT NULL DEFAULT 0,
       ip varchar(32) NOT NULL DEFAULT '',
       area varchar(64) NOT NULL DEFAULT '',
       url varchar(256) NOT NULL DEFAULT '',
       title varchar(256) NOT NULL DEFAULT '',
       refer varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

CREATE INDEX track_index ON track (uid, oid);

\c lcs_main;

ALTER TABLE userinfo ADD addr varchar(64) NOT NULL DEFAULT '';

\c lcs_mtool;

ALTER TABLE stat RENAME TO kol;
ALTER SEQUENCE stat_id_seq RENAME TO kol_id_seq;
CREATE INDEX kol_index ON kol (type, count);
CREATE TABLE visit (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       pv int NOT NULL DEFAULT 0,
       uv int NOT NULL DEFAULT 0,
       dt date DEFAULT current_date,
       PRIMARY KEY (id)
);

CREATE TABLE topref (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       refer varchar(256) NOT NULL DEFAULT 0,
       count int NOT NULL DEFAULT 0,
       dt date DEFAULT current_date,
       PRIMARY KEY (id)
);

CREATE TABLE topurl (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       url varchar(256) NOT NULL DEFAULT 0,
       title varchar(256) NOT NULL DEFAULT 0,
       count int NOT NULL DEFAULT 0,
       dt date DEFAULT current_date,
       PRIMARY KEY (id)
);

CREATE TABLE toparea (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       area varchar(64) NOT NULL DEFAULT '',
       count int NOT NULL DEFAULT 0,
       dt date DEFAULT current_date,
       PRIMARY KEY (id)
);

\i view_mtls.sql;
