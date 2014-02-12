; var bmoon = bmoon || {};
bmoon.appinfo = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.appinfo;
        if (o.inited) return o;

        o.email = $('#email');
        o.emailmod = $('#email-mod');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.appinfo.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.appinfo.init();

        o.email.change(o.modEmail);
        o.email.blur(function() {
            o.email.attr('disabled', 'disabled');
        });
        o.emailmod.click(function() {
            o.email.removeAttr('disabled').focus();
        });
    },

    modEmail: function() {
        var o = bmoon.appinfo.init();

        var p = $(this).parent(),
        email = o.email.val();
        
        if (!$('.VAL_INFO').inputval()) return;

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/oms/edit', {_op: 'mod', email: email}, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
    }
};

$(document).ready(bmoon.appinfo.onready);
