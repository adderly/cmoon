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
