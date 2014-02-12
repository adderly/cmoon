CREATE TABLE appinfo (
       aid int PRIMARY KEY,
       aname varchar(256) NOT NULL DEFAULT '',
       asn varchar(256) NOT NULL DEFAULT '',
       masn varchar(256) NOT NULL DEFAULT '',
       email varchar(256) NOT NULL DEFAULT '',
       state int NOT NULL DEFAULT 0,
       tune int NOT NULL DEFAULT 0,
       pid int NOT NULL DEFAULT 0,    --0: superuser, x: X site's adminer
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now()
);

CREATE INDEX app_index ON appinfo (state);

CREATE TRIGGER tg_uptime_app BEFORE UPDATE ON appinfo FOR EACH ROW EXECUTE PROCEDURE update_time();


CREATE TABLE userinfo (
       uid int,
       uname varchar(256) NOT NULL DEFAULT '',
       aid int,
       aname varchar(256) NOT NULL DEFAULT '',
       ip varchar(32) NOT NULL DEFAULT '',
       addr varchar(64) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now(),
       PRIMARY KEY (uid, aid)
);

CREATE INDEX user_index ON userinfo (aid);

CREATE TRIGGER tg_uptime_user BEFORE UPDATE ON userinfo FOR EACH ROW EXECUTE PROCEDURE update_time();


CREATE TABLE appreset (
       aname varchar(256) NOT NULL DEFAULT '',
       rlink varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       PRIMARY KEY (aname)
);

CREATE FUNCTION merge_appreset(e TEXT, r TEXT) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE appreset SET rlink = r WHERE aname = e;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO appreset(aname, rlink) VALUES (e, r);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END
$$
LANGUAGE plpgsql;
