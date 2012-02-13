ALTER TABLE member ADD COLUMN mnick varchar(256) NOT NULL DEFAULT '';
ALTER TABLE member ADD COLUMN msn varchar(256) NOT NULL DEFAULT '';
ALTER TABLE member ADD COLUMN mmsn varchar(256) NOT NULL DEFAULT '';

CREATE TABLE memory (
    id SERIAL,
    statu int NOT NULL DEFAULT 0,

    title varchar(256) NOT NULL DEFAULT '',
    hint varchar(256) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    previd int NOT NULL DEFAULT 0,
    nextid int NOT NULL DEFAULT 0,
    
    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE TRIGGER tg_uptime_memory BEFORE UPDATE ON memory FOR EACH ROW EXECUTE PROCEDURE update_time();

CREATE TABLE comment (
    id SERIAL,

    type int NOT NULL DEFAULT 0, --0 memory
    statu int NOT NULL DEFAULT 0,

    oid int NOT NULL DEFAULT 0,
    pid int NOT NULL DEFAULT 0,

    ip varchar(64) NOT NULL DEFAULT '',
    addr varchar(256) NOT NULL DEFAULT '',

    author varchar(64) NOT NULL DEFAULT '',
    contact varchar(256) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),    
    PRIMARY KEY (id)
);
CREATE TRIGGER tg_uptime_comment BEFORE UPDATE ON comment FOR EACH ROW EXECUTE PROCEDURE update_time();


CREATE TABLE memberreset (
       mname varchar(256) NOT NULL DEFAULT '',
       rlink varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       PRIMARY KEY (mname)
);

CREATE FUNCTION merge_memberreset(e TEXT, r TEXT) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE memberreset SET rlink = r WHERE mname = e;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO memberreset(mname, rlink) VALUES (e, r);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END
$$
LANGUAGE plpgsql;

CREATE TABLE email (
       id SERIAL,
       state int NOT NULL DEFAULT 0, -- 0 fresh 1 sended
       gotime int NOT NULL DEFAULT 0, -- 0 immediatly 1 minute 2 O clock 3 midnight
       opts varchar(512) NOT NULL DEFAULT '', -- -c /usr/local/etc/email/liuchunsheng.conf -html -s....
       subject varchar(256) NOT NULL DEFAULT '',
       sendto text NOT NULL DEFAULT '',
       cc text NOT NULL DEFAULT '',
       bcc text NOT NULL DEFAULT '',
       content text NOT NULL DEFAULT '',
       checksum varchar(64) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now(),    
       PRIMARY KEY (id)
);

CREATE INDEX email_index ON email (state, gotime);
CREATE TRIGGER tg_uptime_email BEFORE UPDATE ON email FOR EACH ROW EXECUTE PROCEDURE update_time();
