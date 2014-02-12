function liveCS(ape) {
    var ui = bmoon.chat;
    this.initialize = function(opts) {
        opts.pname = opts.pname || 'unknown';
        opts.aname = opts.aname || Cookie.read('aname_esc');
        opts.masn = opts.lcsmasn = Cookie.read('masn');
        ape.opts = opts;
        
        ape.lcsCurrentPipe = null;
        
        if (!ape.opts.aname || !ape.opts.masn) return;

        ape.onRaw("ident", this.rawLcsIdent);
        ape.onRaw("LCS_DEARUSERS", this.rawLcsDearusers);
        ape.onRaw("RAW_RECENTLY", this.rawLcsRecently);
        ape.onRaw("LCS_DATA", this.rawLcsData);

        ape.onError("110", ape.clearSession);
        ape.onError("111", ape.clearSession);
        ape.onError("112", this.needMoreAdmin);
        ape.onError("113", ape.clearSession);

        ape.onError("210", ape.clearSession);
        ape.onError("211", ape.clearSession);
        
        ape.addEvent("userJoin", this.createUser);
        ape.addEvent("userLeft", this.deleteUser);
        ape.addEvent("multiPipeCreate", this.pipeCreate);

        ape.addEvent("load", this.start);

        setTimeout(ui.judgeReady, 6000);
    };

    this.start = function() {
        var opt = {'sendStack': false, 'request': 'stack'};
        ape.start({'uin': ape.opts.aname}, opt);
        if (!ape.options.restore) {
            ape.request.stack.add("LCS_JOINB", {
                'aname': ape.opts.aname, 'masn': ape.opts.masn}, opt);
        }
        ape.request.stack.send();
    };

    this.needMoreAdmin = function() {
        ape.clearSession();
        ui.needMoreAdmin();
    };

    this.pipeCreate = function(pipe, options) {
        // avoid sended pipeCreate event in
        if (pipe.properties.aname == ape.opts.aname && !options.from) {
            ape.lcsCurrentPipe = pipe;
            ui.onready(ape, pipe.getPubid());
            ape.request.stack.add('LCS_DEARUSERS', null);
            ape.request.stack.send();
        }
    };

    this.createUser = function(user, pipe) {
        if (pipe.properties.aname == ape.opts.aname &&
            user.properties.uin != ape.opts.aname) {
            ui.userOn({uname: user.properties.uin, pubid: user.pubid});
        }
    };

    this.deleteUser = function(user, pipe) {
        if (pipe.properties.aname == ape.opts.aname &&
            user.properties.uin != ape.opts.aname) {
            ui.userOff({uname: user.properties.uin});
        }
    };

    this.rawLcsIdent = function(raw, pipe) {
    };

    this.rawLcsDearusers = function(raw, pipe) {
        //var users = [];
        //$.each(raw.data, function(key, val){
        //    users.push(key);
        //});
        //ui.dearUsers(users);
        ui.dearUsers(raw.data);
    };

    this.rawLcsRecently = function(raw, pipe) {
        // some offline & notice message don't have data.to.properties.uin,
        // use data.to_uin instead
        if ((raw.data.from && raw.data.from.properties.uin == ape.opts.aname) ||
            raw.data.to_uin == ape.opts.aname ||
            (raw.data.to && raw.data.to.properties.uin == ape.opts.aname)) {
            var
            type = raw.data.type,
            
            //stm = new Date(raw.time*1000).toString(),
            //tm = stm.match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
            dt = new Date(raw.time*1000),
            y = dt.getFullYear(), m = dt.getMonth()+1, d = dt.getDate(),
            h = dt.getHours(), mt = dt.getMinutes(), s = dt.getSeconds(),
            tm = y+"-"+m+"-"+d+" "+h+":"+mt+":"+s,
            
            from = raw.data.from.properties.uin,
            to = raw.data.to_uin ||
                (raw.data.to && raw.data.to.properties.uin) || "0";
            
            ui.onRecently({from: from, to: to, type: type, tm: tm, data: raw.data});
        }
    };

    this.rawLcsData = function(raw, pipe) {
        // some notice message don't have data.to.properties.uin,
        // use data.to_uin instead
        if (raw.data.to_uin == ape.opts.aname ||
            (raw.data.to && raw.data.to.properties.uin == ape.opts.aname)) {
            var
            type = raw.data.type,
            stm = new Date(raw.time*1000).toString(),
            tm = stm.match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
            from = raw.data.from.properties.uin;
            
            ui.onData({from: from, type: type, tm: tm, data: raw.data});
        }
    };
}
