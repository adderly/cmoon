// liveCS.initialize(ape, {aname: document.domain, debug: debug});

/*
 * 传统意义上的一个普通变量， 不能实例化， 针对单个 document 是全局的
 */
var liveCS = {
    initialize: function(ape, opts) {
        var o = liveCS;
        o.ape = ape;
        // app name
        o.lcsaname = opts.aname || document.domain;

        // user name
        o.lcsuname = Cookie.read("lcs_uname");
        if (o.lcsuname == null) {
            o.lcsuname = bmoon.utl.randomWord(8);
            Cookie.write("lcs_uname", o.lcsuname, {'path': '/', 'duration': 36500});
        }

        // user visit this app's time
        o.lcsutime = Cookie.read("lcs_utime");
        if (!o.lcsutime) {
            o.lcsutime = 1;
            Cookie.write("lcs_utime", "1", {'path': '/', 'duration': 36500});
        } else {
            o.lcsutime = parseInt(o.lcsutime)+1;
            if (!ape.options.restore) {
                Cookie.write("lcs_utime", o.lcsutime, {'path': '/', 'duration': 36500});
            }
        }

        o.lcsPipeName = "livecspipe_" + o.lcsaname;
        o.publicPipe = null;
        o.currentPipe = null;

        ape.onRaw("ident", o.rawLcsIdent);
        ape.onRaw("lcsdata", o.rawLcsData);

        ape.onError("110", ape.clearSession);
        ape.onError("111", ape.clearSession);
        ape.onError("112", ape.clearSession);
        ape.onError("113", ape.clearSession);

        ape.addEvent("userJoin", o.createUser);
        ape.addEvent("userLeft", o.deleteUser);
        ape.addEvent("multiPipeCreate", o.pipeCreate);

        ape.addEvent("load", o.start);
    },

    start: function() {
        var o = liveCS;
        var opt = {'sendStack': false, 'request': 'stack'};
        o.ape.start({'uin': o.lcsuname}, opt);
        if (o.ape.options.restore) {
            o.ape.getSession('currentPipe', function(resp) {
                                 o.setCurrentPipe(resp.data.sessions.currentPipe);
                             }, opt);
        } else {
            o.ape.request.stack.add("LCS_JOIN",
                                    {'aname': o.lcsaname, 'utime': o.lcsutime}, opt);
        }
        o.ape.request.stack.send();
    },

    setCurrentPipe: function(pubid) {
        var o = liveCS;
        o.currentPipe = o.ape.getPipe(pubid);
        o.ape.setSession({'currentPipe': pubid});
    },

    getCurrentPipe: function() {
        var o = liveCS;
        return o.currentPipe;
    },

    pipeCreate: function(pipe, options) {
        var o = liveCS;
        if (pipe.properties.name.toLowerCase() == o.ape.lcsPipeName) {
            o.publicPipe = pipe;
            if (!o.getCurrentPipe()) {
                o.setCurrentPipe(pipe.getPubid());
            }
        }
    },

    createUser: function(user, pipe) {
        var o = liveCS;
        if (pipe.properties.isadmin) {
            o.setCurrentPipe(pipe.getPubid());
        }
    },

    deleteUser: function(user, pipe) {
        var o = liveCS;
        if (pipe.properties.isadmin) {
            o.setCurrentPipe(o.publicPipe.getPubid());
        }
    },

    rawLcsIdent: function(raw, pipe) {
        var o = liveCS;
        var jid = parseInt(raw.data.user.properties.jid);
        // send LCS_VISIT only on session restore
        if (jid && o.ape.options.restore) {
            o.ape.request.send("LCS_VISIT", {'jid': jid, 'url': window.location.href, 'title': window.title});
        }
    },

    rawLcsData: function(raw, pipe) {
        var o = liveCS;
        if (pipe == o.getCurrentPipe()) {
            var msg = unescape(raw.data.msg);
            var uin = raw.data.from.properties.uin;
            var tm = Date(eval(raw.time)).match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0];
            //bmoon.lcs.userSaid(ape, uin, msg, tm);
        }
    }
};
