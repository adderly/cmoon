; var bmoon = bmoon || {};
bmoon.apppass = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.apppass;
        if (o.inited) return o;

        o.asn = $('#asn');
        o.asnr = $('#asn-r');
        o.submit = $('#submit');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.apppass.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.apppass.init();

        o.asnr.blur(o.asnCheck);
        o.submit.click(o.appPass);
    },

    asnCheck: function() {
        var o = bmoon.apppass.init();
        
        if (!$('.VAL_APPPASS').inputval()) return false;
        
        var    asn = o.asn.val(),
        asnr = o.asnr.val(),
        p = $(this).parent();

        $(".vres", p).remove();
        p.removeClass('success').removeClass('error');
        if (asn !== asnr) {
            $('<span class="vres">两次密码不匹配</span>').appendTo(p);
            p.addClass('error');
            return false;
        } else {
            p.addClass('success');
        }
        return true;
    },

    appPass: function() {
        var o = bmoon.apppass.init();

        if (!$('.VAL_APPPASS').inputval()) return;
        if (!o.asnCheck()) return;

        var asn = $.md5($.md5(o.asn.val())),
        p = $(this).parent();

        $(".vres", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/oms/edit', {_op: 'mod', asn: asn}, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
            }
        }, 'json');
    }
};

$(document).ready(bmoon.apppass.onready);
