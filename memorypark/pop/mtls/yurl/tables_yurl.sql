CREATE TABLE com (
       id int PRIMARY KEY,
       name varchar(256) NOT NULL DEFAULT '',
       city varchar(64) NOT NULL DEFAULT '',
       industry varchar(256) NOT NULL DEFAULT '',
       addr varchar(256) NOT NULL DEFAULT '',
       url varchar(256) NOT NULL DEFAULT '',
       body varchar(64) NOT NULL DEFAULT '',
       tel varchar(64) NOT NULL DEFAULT '',
       fax varchar(64) NOT NULL DEFAULT '',
       zipcode varchar(64) NOT NULL DEFAULT '',
       email varchar(64) NOT NULL DEFAULT '',
       prod text NOT NULL DEFAULT '',
       intro text NOT NULL DEFAULT '',
       qq varchar(64) NOT NULL DEFAULT '',
       op int NOT NULL DEFAULT 0
);
