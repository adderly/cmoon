//; if (!bmoon) bmoon = {};
; var bmoon = bmoon || {};
bmoon.chat = {
    version: "1.0",
    adminuser: {},
    ape: {},

    _strAction: function(type, data) {
        var
        msg = decodeURIComponent(data.msg).replace(/<\/?[^>]*>/g, ''),
        r = {
            'send':    msg,
            'msg': '留言说： ' + msg
        };
        
        return r[type];
    },

    // {from: from, type: type, tm: tm, data: data}
    _strMsg: function(data) {
        var o = bmoon.chat.init();

        var uname = data.from == o.ape.opts.uname ? '我': data.from;
        
        return [
            '<div class="item-', data.type, '">',
                '<span class="item-name">',    decodeURIComponent(uname),'</span>',
                '<span class="item-time">',    data.tm,'</span>',
                '<span class="item-content">', o._strAction(data.type, data.data),
                '</span>',
            '</div>'
        ].join('');
    },

    _blink: function() {
        var o = bmoon.chat.init();

        if (!o.maxer.blinkID) {
            o.maxer.blinkID = setInterval(function() {
                o.maxer.toggleClass('dirty');
            }, 500);
        }
    },

    _stopBlink: function() {
        var o = bmoon.chat.init();

        clearInterval(o.maxer.blinkID);
        o.maxer.removeClass('dirty');
        o.maxer.blinkID = 0;
    },

    _getCssV: function(cssv, total) {
        if (bmoon.utl.type(cssv) == 'String' && cssv.match(/.*\%$/)) return total * parseInt(cssv) / 100;
        else return parseInt(cssv);
    },
    
    _rendBox: function(op) {
        var o = bmoon.chat.init();

        if (o.ielow) {
            var sh = $(window).scrollTop(),
            sl = $(window).scrollLeft(),
            wh = $(window).height(),
            ww = $(window).width(),
            rh = o.chatbody.height(),
            rw = o.chatbody.width(),
            pos = o.ape.opts.pos,
            top = pos.top === undefined ? (sh + wh - o._getCssV(pos.bottom, wh) - rh) : (sh + o._getCssV(pos.top, wh)),
            left = pos.left === undefined ? (sl + ww - o._getCssV(pos.right, ww) - rw) : (sl + o._getCssV(pos.left, ww));

            op = $.extend({
                ani: true
            }, op || {});
            o.chatbody.css('position', 'absolute');
            if (op.ani) o.chatbody.animate({top: top, left: left}, 10);
            else o.chatbody.css({top: top, left: left});
        }
    },

    debug: function(msg) {
        //$('<div>'+ msg +'</div>').appendTo('body');
    },
    
    init: function(ape) {
        var o = bmoon.chat,
        op = ape ? ape.opts : {};
        
        if (o.inited) return o;
        o.inited = true;

        var    rmdhtml = $.browser.msie? '<bgsound id="kol-lcs-remind"></bgsound>': '<audio id="kol-lcs-remind"></audio>',
        html = [
            '<div id="kol-lcs">',
                '<div id="kol-lcs-max">',
                    '<div class="mider" title="缩小"></div>',
                    '<div class="hint">客服当前离线，留言功能开启。</div>',
                    '<div class="msgs">',
                        '<div class="recently"></div><div class="data"></div>',
                    '</div>',
                    '<div class="bot">输入消息：',
                        '<span><input type="checkbox" id="kol-lcs-remind-sw" checked="checked" /> 提示音</span>',
                    '</div>',
                    '<textarea id="kol-lcs-input"></textarea>',
                '</div>',
                '<div id="kol-lcs-mid"></div>',
                rmdhtml,
            '</div>'
        ].join('');
        
        o.chatbody = $('#kol-lcs');

        // application don't write kol-lcs, append it.
        if (!o.chatbody.length) {
            $('body').append(html);
            o.chatbody = $('#kol-lcs');
        }

        o.ape = ape;
        o.max = $('#kol-lcs-max');
        o.mid = $('#kol-lcs-mid');
        o.rendo = o.mid;
        
        // maxer mider miner
        o.mider = $('.mider', o.max);
        if (op.minable) {
            o.min = $('<div/>').attr('id', 'kol-lcs-min').appendTo(o.chatbody);
            o.maxer = $('<div/>').addClass('maxer').appendTo(o.mid);
            $('<div/>').addClass('miner').attr('title', '关闭').appendTo(o.mid);
            if (o.ape.opts.hidemidOnMax)
                $('<div/>').addClass('miner').attr('title', '关闭').insertBefore(o.mider);
            o.miner = $('.miner', o.chatbody);
        } else o.maxer = o.mid;

        o.msglist = $('.msgs', o.max);
        o.recentbox = $('.recently', o.msglist);
        o.databox = $('.data', o.msglist);
        o.hint = $('.hint', o.max);
        
        o.reminder = $('#kol-lcs-remind')[0];
        o.rmdsw = $('#kol-lcs-remind-sw');
        o.msginput = $('#kol-lcs-input');

        o.ielow = bmoon.utl.ie() && bmoon.utl.ie() < 7;

        o.initUI();
        
        return o;
    },

    initUI: function() {
        var o = bmoon.chat.init();

        var ui = o.ape.opts.restoreUI ? $.cookie('lcs_ui') : o.ape.opts.defaultUI;

        switch (ui) {
        case 'max':
            o.rendo = o.max;
            break;
        case 'min':
            o.rendo = o.min ? o.min: o.mid;
            break;
        case 'mid':
        default:
            o.rendo = o.mid;
            break;
        }

        o.max.hide();
        o.mid.hide();
        o.min && o.min.hide();
        if (o.rendo == o.max && !o.ape.opts.hidemidOnMax) o.mid.show();
        o.rendo.fadeIn();

        if (o.ielow) {
            // ielow double margin on float node. set display inline to fix it
            o.hint.css('display', 'inline');
            o.mider.css('display', 'inline');
            o.ape.opts.minable && o.maxer.css('display', 'inline');
            o.ape.opts.minable && o.miner.css('display', 'inline');

            // ielow position: fixed bug. set absolute and scrool to fix it
            o._rendBox({ani: false});
            $(window).scroll(o._rendBox);
            $(window).resize(o._rendBox);
        } else o.chatbody.css(o.ape.opts.pos);

        if ($.inArray(o.ape.opts.maxevent, ['click', 'mouseenter']) == -1)
            o.ape.opts.maxevent = 'click';
        if (o.rendo == o.max && !o.ape.opts.hidemidOnMax)
            o.maxer.bind(o.ape.opts.maxevent, o.closeChat);
        else
            o.maxer.bind(o.ape.opts.maxevent, o.openChat);
        if (o.ape.opts.maxevent == 'mouseenter') {
            o.maxer.mouseover(function() {o.maxer.addClass('mouseover');});
            o.maxer.mouseout(function() {o.maxer.removeClass('mouseover')});
        }
        o.mider.click(o.closeChat);
        o.msginput.bind('keydown', 'return', o.msgSend);

        o.miner && o.miner.click(function() {
            o.max.hide();
            o.mid.hide();
            o.min.fadeIn();
            o._rendBox({ani: false});
            $.cookie('lcs_ui', 'min');
        });
        o.min && o.min.click(function() {
            o.min.hide();
            !o.ape.opts.hidemidOnMax && o.mid.show();
            o.rendo != o.min && o.rendo.fadeIn();
            o._rendBox({ani: false});
            if (o.max.css('display') == 'none') $.cookie('lcs_ui', 'mid');
            else $.cookie('lcs_ui', 'max');
        })
    },

    updateUI: function(opts) {
        var o = bmoon.chat.init();

        o.ape.opts = $.extend(o.ape.opts, opts || {});

        if (opts.pos) {
            $.each(o.ape.opts.pos, function(k, v){
                if (!v.match(/.*\%$/))
                    o.ape.opts.pos[k] = parseInt(v);
            });
            if (o.ielow) o._rendBox();
            else o.chatbody.css(o.ape.opts.pos);
        }
        if (opts.css) {
        }
    },
    
    openChat: function() {
        var o = bmoon.chat.init();

        if (o.ape.opts.hidemidOnMax) o.mid.hide();
        else if (o.ape.opts.maxevent == 'click') {
            // toggle open & close on click
            o.maxer.unbind(o.ape.opts.maxevent).bind(o.ape.opts.maxevent, o.closeChat);
        }
        
        o.max.fadeIn();
        o.rendo = o.max;
        o._rendBox({ani: false});
        o.msginput.focus();
        
        o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
        o._stopBlink();
        $.cookie('lcs_ui', 'max');
    },

    closeChat: function() {
        var o = bmoon.chat.init();

        o.max.hide();
        if (o.ape.opts.maxevent == 'mouseenter') {
            // prevent max after mided on mouse in mid
            o.maxer.unbind('mouseenter');
            setTimeout(function() {
                if (o.maxer.hasClass('mouseover'))
                    o.maxer.mouseout(function() {o.maxer.mouseenter(o.openChat);});
                else o.maxer.mouseenter(o.openChat);
            }, 50);
        } else if (!o.ape.opts.hidemidOnMax) {
            // toggle open & close on click
            o.maxer.unbind(o.ape.opts.maxevent).bind(o.ape.opts.maxevent, o.openChat);
        }
        o.mid.show();
        o.rendo = o.mid;
        o._rendBox({ani: false});
        $.cookie('lcs_ui', 'mid');
    },

    msgSend: function() {
        var o = bmoon.chat.init();
        
        var mv = o.msginput.val(),
        pipe = o.ape.lcsCurrentPipe,
        type = o.adminuser ? 'send': 'msg',
        html = o._strMsg({
            from: o.ape.opts.uname,
            type: type,
            tm: Date().match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
            data: {msg: mv}
        });

        if (!mv.length) return false;
        if (mv.length > 256) {
            var ot = o.hint.html();
            o.hint.html('聊天信息不要超过 256 个字。');
            setTimeout(function(){o.hint.html(ot);}, 2000);
            return false;
        }
        o.msginput.val('');

        $(html).appendTo(o.databox);
        o.msglist[0].scrollTop = o.msglist[0].scrollHeight;

        if (o.adminuser.aname && pipe) {
            pipe.request.send('LCS_SEND', {msg: mv});
        } else {
            o.ape.request.send('LCS_MSG', {
                uname: o.ape.opts.aname ? o.ape.opts.aname : o.ape.opts.pname,
                msg: mv
            });
        }
        
        o.soundRemind('send');
        return false;
    },

    adminOn: function(data) {
        var o = bmoon.chat.init();

         o.debug(data.pname + ' 的管理员 ' +data.aname + ' 上线了');
        o.hint.html(data.aname + ' 当前在线。');
        o.adminuser = data;
        
        o.soundRemind('login');
    },

    adminOff: function() {
        var o = bmoon.chat.init();

         o.debug('管理员走开了');
        o.hint.html('客服当前离线，留言功能开启。');
        o.adminuser = {};
    },
    
    // {from: from, type: type, tm: tm, data: data}
    onData: function(data) {
        var o = bmoon.chat.init();

        var
        uname = data.from,
        html = o._strMsg(data);

        $(html).appendTo(o.databox);
        if (o.max.css('display') != 'none') {
            o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
        } else {
            o._blink();
        }

        o.soundRemind('receive');
    },

    // {from: from, to: to, type: type, tm: tm, data: data}
    onRecently: function(data) {
        var o = bmoon.chat.init();

        var html = o._strMsg(data);
        
        $(html).prependTo(o.recentbox);
        o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
    },

    soundRemind: function(type) {
        var o = bmoon.chat.init();

        if (o.rmdsw.attr('checked') == true) {
            o.reminder.src = 'http://www.bomdoo.com/obj/audio/'+type+'.wav';
            if (!$.browser.msie) {
                o.reminder.load();
                o.reminder.play();
            }
        }
    }
};
