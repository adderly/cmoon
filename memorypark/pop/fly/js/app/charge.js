; var bmoon = bmoon || {};
bmoon.appcharge = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.appcharge;
        if (o.inited) return o;

        o.mpre = $('#mpre');
        o.link = $('#cg-link');
        o.submit = $('#submit');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.appcharge.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.appcharge.init();

        o.mpre.blur(function() {
            o.link.fadeIn();
            o.submit.fadeIn();
        });
        o.submit.click(o.charge);
    },

    charge: function() {
        var o = bmoon.appcharge.init();

        var mpre = o.mpre.val(),
        p = $(this).parent();

        if (!$('.VAL_CHARGE').inputval()) return;

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/app/charge', {_op: 'add', mpre: mpre}, function(data){
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                $('<span class="vres">申请充值成功，播豆工作人员会在1个工作日内确认你的付款。</span>').appendTo(p);
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
    }
};

$(document).ready(bmoon.appcharge.onready);
