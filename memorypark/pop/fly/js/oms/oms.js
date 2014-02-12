; var bmoon = bmoon || {};
bmoon.oms = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.oms;
        if (o.inited) return o;

        o.scroll = $(".scrollable").scrollable({api: true});
        o.navi = $(".scrollable").navigator({api: true});
        o.icneeda = $('#ic-needa');
        o.imindi = $('#chat-indicator');
        o.needa = $('#chat-needa');

        o.browserhint = $('#browser-hint');
        o.browserlater = $('#browser-later');
        o.ielow = bmoon.utl.ie() && bmoon.utl.ie() < 7;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.oms.init();

        o.bindClick();

        bmoon.utl.after(function() {
            if (mgd.appinfo.tune & 8) {
                o.imindi.fadeOut();
                o.needa.fadeIn();
                return;
            }
            o.imindi.fadeIn();
            var opts = {
                pname: mgd.appinfo.pname,
                aname: mgd.appinfo.aname,
                masn: mgd.appinfo.masn
            },
            client = new APE.Client();
            client.load({
                identifier: 'kol_backend',
                transport: 2,
                complete: function(ape) {
                    new liveCS(ape).initialize(opts);
                }
            });
        }, 'mgd.appinfo !== undefined', 10);
        o.ielow && !$.cookie('lcs_bs_ignore') && o.browserhint.fadeIn('slow');
    },

    bindClick: function() {
        var o = bmoon.oms.init();

        $('#im-users li .remove').click(o.removeCamer);
        $('#im-msgs').mnscrollpage({trigpos: 'top', ppage: 1, callback: o.getMessage});
        o.icneeda.click(function() {
            var p = $(this).parent();

            $.post('/json/oms/edit', {
                _op: 'mod',
                aname: mgd.appinfo.pname,
                tuneop: 0,
                tune: 8
            }, function(data) {
                p.fadeOut();
            }, 'json');
        });

        o.browserlater.click(function() {
            o.browserhint.fadeOut();
            $.cookie('lcs_bs_ignore', '1', {expires: 1});
        });
    },

    removeCamer: function() {
        var o = bmoon.oms.init();

        var
        c = $(this),
        p = c.parent(),
        uname = p.attr('uname');

        $.post('/json/oms/camer', {_op: 'del', uname: uname}, function(data) {
            if (data.success == '1') {
                p.remove();
            } else {
                alert(data.errmsg || '删除操作失败，请稍后再试');
            }
        }, 'json');
    },

    getMessage: function(pg) {
        var o = bmoon.oms.init();

        var p = bmoon.chat;
        
        p.getMessage(p.cUserID, p.ape.opts.aname, pg);
    }
};

$(document).ready(bmoon.oms.onready);
