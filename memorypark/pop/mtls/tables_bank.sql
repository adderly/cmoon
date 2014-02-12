CREATE TABLE bank (
    aid int PRIMARY KEY,
    aname varchar(256) NOT NULL DEFAULT '',
    msum int NOT NULL DEFAULT 0,
    mpre int NOT NULL DEFAULT 0,
    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now()
);

CREATE TRIGGER tg_uptime_app BEFORE UPDATE ON bank FOR EACH ROW EXECUTE PROCEDURE update_time();

CREATE FUNCTION merge_bank_pre(a TEXT, k INT, v INT) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE bank SET mpre = mpre+v WHERE aid = k;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO bank (aid, aname, mpre) VALUES (k, a, v);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END
$$
LANGUAGE plpgsql;


CREATE TABLE bill (
    id SERIAL,
    aid int NOT NULL DEFAULT 0,
    aname varchar(256) NOT NULL DEFAULT '',
    btype int NOT NULL DEFAULT 0,
    fee int NOT NULL DEFAULT 0,
    remark varchar(256) NOT NULL DEFAULT '',
    intime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);

CREATE INDEX bill_index ON bill (aid, btype);
