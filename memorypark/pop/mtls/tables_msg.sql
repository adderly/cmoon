CREATE TABLE msg (
       id SERIAL,
       mfrom varchar(256) NOT NULL DEFAULT '',
       mto varchar(256) NOT NULL DEFAULT '',
       mtype int NOT NULL DEFAULT 0, -- 0: send 1: offline_msg 2: join 3: visit 4: left
       mraw varchar(1024) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       PRIMARY KEY (id)
);
