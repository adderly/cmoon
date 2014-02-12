; var bmoon = bmoon || {};
bmoon.appreset = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.appreset;
        if (o.inited) return o;

        o.aname = $('#aname');
        o.submit = $('#submit');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.appreset.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.appreset.init();

        //o.aname.blur(o.appCheck);
        o.submit.click(o.appReset);
    },

    appCheck: function() {
        var o = bmoon.appreset.init();
        
        var
        aname = o.aname.val(),
        p = $(this).parent();
        
        if (aname.length > 0) {
            $(".vres", p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            $.getJSON('/json/app/exist', {aname: aname}, function(data) {
                p.removeClass('loading');
                if (data.success == 1) {
                    if (data.exist == 0) {
                        $('<span class="vres">'+ data.msg +'</span>').appendTo(p);
                        p.addClass('error');
                    } else {
                        p.addClass('success');
                    }
                } else {
                    p.addClass('error');
                    $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
                }
            });
        }
    },

    appReset: function() {
        var o = bmoon.appreset.init();

        if (!$('.VAL_RESETAPP').inputval()) return;

        var aname = o.aname.val(),
        p = $(this).parent();

        $(".vres", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.getJSON('/json/app/reset', {aname: aname}, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                $('<span class="vres">邮件发送成功，一分钟左右送达。</span>').appendTo(p);
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
            }
        });
    }
};

$(document).ready(bmoon.appreset.onready);
