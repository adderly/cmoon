CREATE TABLE kol (
       id SERIAL,
       type int NOT NULL DEFAULT 0,
       count int NOT NULL DEFAULT 0,
       intime timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

CREATE INDEX kol_index ON kol (type, count);


CREATE TABLE visit (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       pv int NOT NULL DEFAULT 0,
       uv int NOT NULL DEFAULT 0,
       --dt date DEFAULT current_date,
       dt timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

CREATE TABLE topref (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       refer varchar(256) NOT NULL DEFAULT 0,
       count int NOT NULL DEFAULT 0,
       dt timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

CREATE TABLE topurl (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       url varchar(256) NOT NULL DEFAULT 0,
       title varchar(256) NOT NULL DEFAULT 0,
       count int NOT NULL DEFAULT 0,
       dt timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

CREATE TABLE toparea (
       id SERIAL,
       aid int NOT NULL DEFAULT 0,
       area varchar(64) NOT NULL DEFAULT '',
       count int NOT NULL DEFAULT 0,
       dt timestamp DEFAULT now(),
       PRIMARY KEY (id)
);

--CREATE TABLE toppv (
--       id SERIAL,
--       aid int NOT NULL DEFAULT 0,
--       pv int NOT NULL DEFAULT 0,
--       count int NOT NULL DEFAULT 0,
--       dt date DEFAULT current_date,
--       PRIMARY KEY (id)
--);
