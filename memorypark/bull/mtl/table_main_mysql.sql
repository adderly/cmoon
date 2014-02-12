CREATE TABLE member (
    mname varchar(200) NOT NULL DEFAULT '',
    msn varchar(200) NOT NULL DEFAULT '',
    mmsn varchar(200) NOT NULL DEFAULT '',
    statu smallint NOT NULL DEFAULT 0,
    verify smallint NOT NULL DEFAULT 0,

    phone varchar(100) NOT NULL DEFAULT '',
    contact varchar(100) NOT NULL DEFAULT '',

    intime timestamp DEFAULT now(),
    PRIMARY KEY (mname)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE paper (
    id SERIAL,
    pid int NOT NULL DEFAULT 0,
    statu smallint NOT NULL DEFAULT 0,

    title varchar(400) NOT NULL DEFAULT '',
    keyword varchar(500) NOT NULL DEFAULT '',
    des varchar(500) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    uptime timestamp DEFAULT now() ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE nav (
	pos SERIAL,
    statu smallint NOT NULL DEFAULT 0,
	pid int NOT NULL DEFAULT 0,
    PRIMARY KEY(pos)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
