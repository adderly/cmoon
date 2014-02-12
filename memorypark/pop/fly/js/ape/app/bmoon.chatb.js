//; if (!bmoon) bmoon = {};
; var bmoon = bmoon || {};
bmoon.chat = {
    version: '1.0',
    ape: {},
    groupPubid: null,
    cUserID: '0',
    usersOn: ["0"],
    usersFetched: ["0"],
    usersNew: ["0"],
    usersTotalMsg: {},
    usersLoadPage: {},

    debug: function(msg) {
        //$('<div>'+ msg +'</div>').appendTo('body');
        //console.log(msg);
    },
    
    init: function(ape) {
        var o = bmoon.chat;

        if (o.inited) return o;
        o.inited = true;

        var rmdhtml = $.browser.msie? '<bgsound id="remind-sound"></bgsound>': '<audio id="remind-sound"></audio>';
        
        $(rmdhtml).appendTo('body');
        o.imbox = $('#chat-box');
        o.imindi = $('#chat-indicator');
        o.needa = $('#chat-needa');
        o.m = $('#chat-msg-text');
        o.btm = $('#chat-msg-submit');
        o.usersTop = $('#im-users .items');
        o.userlist = $('#im-users ul:last');
        o.usersPerTab = 15;
        o.msglist = $('#im-msgs');
        o.stat = $('#im-stat');
        o.reminder = $('#remind-sound')[0];
        o.rmdsw = $('#remind-sound-sw');
        
        o.ape = ape;

        return o;
    },

    _nodeUser: function(uname, create) {
        var o = bmoon.chat.init();

        var
        r = $('#im-user-' + uname),
        html = '<li id="im-user-'+uname+'" uname="'+uname+'">'+
            '<span class="uname">'+uname+'</span><span class="place"></span>' +
            '</li>';
        
        if (!r.length && create) {
            if (o.userlist.children().length >= o.usersPerTab) {
                if (bmoon.oms.scroll) {
                    //bmoon.oms.scroll.addItem($('<ul></ul>')).end();
                    bmoon.oms.scroll.addItem($('<ul></ul>'));
                    o.userlist = $('#im-users ul:last');
                } else {
                    o.userlist = $('<ul></ul>').appendTo(o.usersTop);
                }
            }
            r = $(html).appendTo(o.userlist).click(o.openChat);
        }
        return r;
    },

    _nodeMsg: function(uname, create) {
        var o = bmoon.chat.init();
        
        var
        r = $('#im-msg-' + uname),
        html = [
            '<div id="im-msg-', uname, '">',
                '<div class="recently"></div>',
                '<div class="data"></div>',
            '</div>'
        ].join('');
        
        if (!r.length && create) {
            r = $(html).appendTo(o.msglist);
            if (o.cUserID != uname) r.hide();
        }
        return r;
    },

    _strAction: function(type, data) {
        if (data.ref) data.sref = '通过 ' + decodeURIComponent(data.ref);
        else data.sref = '';
        
        var
        msg = decodeURIComponent(data.msg).replace(/<\/?[^>]*>/g, ''),
        r = {
            'join':
            data.sref + ' 来到本站, 访问了页面 <a target="_blank" href="'+
                decodeURIComponent(data.url)+'">'+
                decodeURIComponent(data.title)+'</a>',
            
            'visit':
            '打开了页面 <a target="_blank" href="'+
                decodeURIComponent(data.url)+'">'+
                decodeURIComponent(data.title)+'</a>',

            'left': '离开了本站',
            'send': msg,
            'msg': '留言说： ' + msg
        };
        return r[type];
    },

    // {from: from, type: type, tm: tm, data: data}
    _strMsg: function(data) {
        var o = bmoon.chat.init();

        var uname = data.from == o.ape.opts.aname ? '我': data.from;
        
        return [
            '<div class="item-', data.type, '">',
                '<span class="item-name">',    decodeURIComponent(uname),'</span>',
                '<span class="item-time">',    data.tm,'</span>',
                '<span class="item-content">', o._strAction(data.type, data.data),
                '</span>',
            '</div>'
        ].join('');
    },

    _strMsgDivide: function(pg) {
        var o = bmoon.chat.init();

        return '<div class="item-divide">第 '+pg+' 页历史消息</div>';
    },

    onready: function(ape, pubid) {
        var o = bmoon.chat.init(ape);

        o.groupPubid = pubid;
        o.bindClick();
        o.imbox.removeClass('loading');
        o.imindi.hide();
    },

    needMoreAdmin: function() {
        var o = bmoon.chat.init();

        o.imindi.fadeOut();
        o.needa.fadeIn();
    },

    judgeReady: function() {
        var o = bmoon.chat.init({});

        if (o.imbox.hasClass('loading')) {
            o.imindi.text('数据加载失败，请联系客服或稍后再试').addClass('text-error');
        }
    },

    bindClick: function() {
        var o = bmoon.chat.init();

        o.m.bind('keydown', 'return', o.msgSend);
        o.btm.click(o.msgSend);
        $('.im-users li').click(o.openChat);
    },

    msgSend: function() {
        var o = bmoon.chat.init();
        
        var
        mv = o.m.val(),
        pipe = o.ape.lcsCurrentPipe,
        type = $.inArray(o.cUserID, o.usersOn) != -1 ? 'send': 'msg',
        databox = $('.data', o._nodeMsg(o.cUserID, true)),
        html = o._strMsg({
            from: o.ape.opts.aname,
            type: type,
            tm: Date().match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
            data: {msg: mv}
        });

        if (!mv.length) return false;
        if (mv.length > 256) {
            var ot = o.stat.html();
            o.stat.html('聊天信息不要超过 256 个字。');
            setTimeout(function(){o.stat.html(ot);}, 2000);
            return false;
        }
        o.m.val('');

        $(html).appendTo(databox);
        o.msglist[0].scrollTop = o.msglist[0].scrollHeight;

        if (pipe && type == 'send') {
            pipe.request.send('LCS_SEND', {msg: mv});
        } else {
            o.ape.request.send('LCS_MSG', {uname: o.cUserID, msg: mv, imAdmin: 1});
        }

        o.soundRemind('send');
        return false;
    },

    openChat: function() {
        var o = bmoon.chat.init();
        
        var
        c = $(this),
        id = c.attr('uname'),
        pubid = c.attr('pubid'),
        userbox = o._nodeUser(o.cUserID, false),
        msgbox = o._nodeMsg(o.cUserID, false);

        userbox.removeClass('cur').click(o.openChat);
        msgbox.hide();
        o.cUserID = id;
        
        c.removeClass('dirty').addClass('cur').unbind('click');
        o._nodeMsg(id, true).fadeIn();
        o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
        if (id == '0') {
            o.stat.html("给所有在线访客发送消息");
        } else {
            o.stat.html("与 " + id + " 对话");
        }

        if ($.inArray(id, o.usersFetched) == -1) {
            o.getMessage(id, o.ape.opts.aname, 1);
            o.usersFetched.push(id);
        }

        if (pubid && pubid.length) {
            o.ape.lcsCurrentPipe = o.ape.getPipe(pubid);
        } else if (o.cUserID == '0') {
            o.ape.lcsCurrentPipe = o.ape.getPipe(o.groupPubid);
        } else {
            o.ape.lcsCurrentPipe = null;
        }
    },

    // so tired. 调了一天， 几个需要解释的地方：
    // 1, 使用 getJSON，加上 JsonCallback=? 参数是为了能让cgi输出正确的json回调形式，实现夸域
    // 2, 由于数据接口里存的是整个 RAW (自己组装多级 json raw 非常麻烦)，
    //    所以数据接口返回的数据是一个字符串数组，而不是 ape 形式的对象数组
    //      数据接口：   ["{\"time\":\"1286291022\",\"raw\":\"RAW_RECENTL
    //      ape返回：   [{"time":"1286353563","raw":"LCS_DATA
    //    所以不能直接调用 Ape.transport.read(data) 或者 o.ape.transport.read(data)
    // 3, $.each 内部不能直接调用 Ape.transport.read('[' +v+ ']')
    //    是因为read() ---> parseResponse() 会干扰 ape.check() 造成请求混乱
    getMessage: function(name, aname, pg) {
        var o = bmoon.chat.init();

        // we use usersLoadPage[name] instead of pg. because we have soooo many pg
        pg = bmoon.utl.type(o.usersLoadPage[name]) == 'Number' ? o.usersLoadPage[name]: 0;
        
        var
        recentbox = $('.recently', o._nodeMsg(name, true)),
        html = o._strMsgDivide(pg+1);
        
        o.debug("get " + name + "'s msg page" + parseInt(pg + 1));
        
        if (bmoon.utl.type(o.usersTotalMsg[name]) != 'Number' || (o.usersTotalMsg[name] / 15) > pg) {
            $.getJSON('http://www.bomdoo.com/json/msg?JsonCallback=?',
                      {name: name, name2: aname, _npp: 15, _npg: pg+1},
                      function(data) {
                          if (data.success == 1 && bmoon.utl.type(data.raws) == 'Array') {
                              $.each(data.raws, function(i, v) {
                                  var raw = JSON.parse(v);
                                  if (raw) {
                                      o.ape.callRaw(raw);
                                  }
                              });
                              $(html).prependTo(recentbox);
                              
                              if (data.ntt) o.usersTotalMsg[name] = parseInt(data.ntt);
                              if (bmoon.utl.type(o.usersLoadPage[name]) == 'Number') o.usersLoadPage[name] += 1;
                              else o.usersLoadPage[name] = 1;
                          }
                      });
        }
    },

    // {defTxg: 1, eoogG: 1}
    dearUsers: function(data) {
        var o = bmoon.chat.init();

        if (bmoon.utl.type(data) == 'Array') {
            $.each(data, function(i, val){
                if (bmoon.utl.type(val) == 'Object') {
                    $.each(val, function(k, v) {
                        var c = o._nodeUser(k, true);
                        if (k != o.cUserID) c.addClass('dirty');
                        o.postUserDirty(k);
                    });
                } else if (bmoon.utl.type(val) == 'String') {
                    var c = o._nodeUser(val, true);
                    if (val != o.cUserID) c.addClass('dirty');
                    o.postUserDirty(val);
                }
            });
        }
    },
    
    // {uname: 'defTxg', pubid: 'esfes323sdfssdf32r'}
    userOn: function(data) {
        var o = bmoon.chat.init();

        var tc = o._nodeUser(data.uname, false),
        c = o._nodeUser(data.uname, true);
        
        c.removeClass('off').addClass('on');
        c.attr('pubid', data.pubid);
        o.usersOn.push(data.uname);

        if (o.cUserID == data.uname) {
            o.ape.lcsCurrentPipe = o.ape.getPipe(data.pubid);
        }

        if (!tc.length) o.usersNew.push(data.uname);

        // forward online user event they aren't dirty
        o.postUserDirty(data.uname);
    },

    // {uname: user.properties.uin}
    userOff: function(data) {
        var o = bmoon.chat.init();

        var
        userbox = o._nodeUser(data.uname, false),
        databox = $('.data', o._nodeMsg(data.uname, false)),
        html = o._strMsg({
            from: data.uname,
            type: 'left',
            tm: Date().match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
            data: {}
        });
        
        if (userbox.length) {
            userbox.removeClass('on').addClass('off').attr('pubid', '');
        }
        o.usersOn.splice($.inArray(data.uname, o.usersOn), 1);

        // for history raw, use event_onleft
        /*
        $(html).appendTo(databox);
        if (o.cUserID == data.uname) {
            o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
        }
        */
    },

    // {from: from, type: type, tm: tm, data: data}
    onData: function(data) {
        var o = bmoon.chat.init();

        var
        uname = data.from,
        userbox = o._nodeUser(uname, true),
        databox = $('.data', o._nodeMsg(uname, true)),
        html = o._strMsg(data);

        if (data.type == 'send') {
            o.soundRemind('receive');
        } else if (data.type == 'join') {
            o.soundRemind('login');
            // TODO, duplicated request?
            $.getJSON('/json/place', {ip: data.data.ip}, function(rdata) {
                if(rdata.success == '1') {
                    $('.place', userbox).text(rdata[0].c);
                }
            });
        } else if (data.type == 'left') {
            o.soundRemind('logout');
        }
        
        
        if (o.cUserID != uname) {
            userbox.addClass('dirty');
            o.postUserDirty(uname);
            // avoid double messages appear
            if ($.inArray(uname, o.usersNew) != -1)
                o.usersFetched.push(uname);
        }
        
        $(html).appendTo(databox);
        if (o.cUserID == uname) {
            o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
        }
    },

    // {from: from, to: to, type: type, tm: tm, data: data}
    onRecently: function(data) {
        var o = bmoon.chat.init();

        var
        uname = data.from == o.ape.opts.aname ? data.to: data.from,
        userbox = o._nodeUser(uname, true),
        recentbox = $('.recently', o._nodeMsg(uname, true)),
        html = o._strMsg(data);
        
        $(html).prependTo(recentbox);
    },

    soundRemind: function(type) {
        var o = bmoon.chat.init();

        if (o.rmdsw.attr('checked') == true) {
            o.reminder.src = '/obj/audio/'+type+'.wav';
            if (!$.browser.msie) {
                o.reminder.load();
                o.reminder.play();
            }
        }
    },

    postUserDirty: function(uname) {
        var o = bmoon.chat.init();

        var userbox = o._nodeUser(uname, false),
        userboxoff = $('#im-users ul li.off:not(.dirty):first');

        if (userbox.length && userboxoff.length) {
            var    up = userbox.parent();
            
            if (up.index() >= userboxoff.parent().index() ||
                userbox.index() > userboxoff.index()) {
                userbox.insertBefore(userboxoff);
                userboxoff.appendTo(up);
            }
        }
    }
};
