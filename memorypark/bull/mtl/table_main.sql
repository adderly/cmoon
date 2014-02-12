CREATE TABLE member (
    mname varchar(200) NOT NULL DEFAULT '',
    msn varchar(200) NOT NULL DEFAULT '',
    mmsn varchar(200) NOT NULL DEFAULT '',
    statu smallint NOT NULL DEFAULT 0,  --0 normal, 1 pause, 2 delete
    verify smallint NOT NULL DEFAULT 0, --90 admin, ..., 100 root

    phone varchar(100) NOT NULL DEFAULT '',
    contact varchar(100) NOT NULL DEFAULT '',

    intime timestamp DEFAULT now(),
    PRIMARY KEY (mname)
);
CREATE INDEX member_index ON member (verify);

CREATE TABLE paper (
    id SERIAL,
    pid int NOT NULL DEFAULT 0,
    statu smallint NOT NULL DEFAULT 0, --0 normal, 1 pause, 2 delete

    title varchar(400) NOT NULL DEFAULT '',
    keyword varchar(500) NOT NULL DEFAULT '',
    des varchar(500) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE INDEX paper_index ON paper (pid, statu);
CREATE TRIGGER tg_uptime_paper BEFORE UPDATE ON paper FOR EACH ROW EXECUTE PROCEDURE update_time();

CREATE TABLE nav (
	pos SERIAL,
    statu smallint NOT NULL DEFAULT 0, --0 normal, 1 pause, 2 delete
	pid int NOT NULL DEFAULT 0,
    PRIMARY KEY(pos)
);

CREATE TABLE mp3 (
    id SERIAL,
    mid int NOT NULL DEFAULT 0,
    title varchar(300) NOT NULL DEFAULT '',
    PRIMARY KEY (id)
);
