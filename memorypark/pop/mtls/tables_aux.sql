CREATE TABLE blog (
       id SERIAL,
       title varchar(256) NOT NULL DEFAULT '',
       content text NOT NULL DEFAULT '',
       state int NOT NULL DEFAULT 0,
       author varchar(256) NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now(),    
       PRIMARY KEY (id)
);
       
CREATE INDEX blog_index ON blog (state);
CREATE TRIGGER tg_uptime_blog BEFORE UPDATE ON blog FOR EACH ROW EXECUTE PROCEDURE update_time();


CREATE TABLE comment (
       id SERIAL,
       type int NOT NULL DEFAULT 0, -- 0 blog, 1 improve
       state int NOT NULL DEFAULT 0,
       oid int NOT NULL DEFAULT 0,
       pid int NOT NULL DEFAULT 0,
       ip varchar(256) NOT NULL DEFAULT '',
       addr varchar(256) NOT NULL DEFAULT '',
       author varchar(256) NOT NULL DEFAULT '',
       content text NOT NULL DEFAULT '',
       intime timestamp DEFAULT now(),
       uptime timestamp DEFAULT now(),    
       PRIMARY KEY (id)
);

CREATE INDEX comment_index ON comment (type, state, oid, pid);
CREATE TRIGGER tg_uptime_comment BEFORE UPDATE ON comment FOR EACH ROW EXECUTE PROCEDURE update_time();


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

-- OWN Aggregate Functions
CREATE AGGREGATE str_concat(
  basetype    = text,
  sfunc       = textcat,
  stype       = text,
  initcond    = ''
);


-- zero (personal help)
CREATE TABLE improve (
    id SERIAL,
    aid int NOT NULL DEFAULT 0,
    aname varchar(256) NOT NULL DEFAULT '',
    state int NOT NULL DEFAULT 0,
    title varchar(256) NOT NULL DEFAULT '',
    content varchar(1024) NOT NULL DEFAULT '',
    contact varchar(64) NOT NULL DEFAULT '',
    intime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);

CREATE INDEX improve_index ON improve (aid, state);
