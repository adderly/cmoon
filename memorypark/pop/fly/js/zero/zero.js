; var bmoon = bmoon || {};
bmoon.zero = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.zero;
        if (o.inited) return o;

        o.addoverlay = $('a[rel="#addoverlay"]').overlay({mask: '#999', api: true});
        o.submit = $('#submit');
        o.title = $('#title');
        o.content = $('#content');
        o.contact = $('#contact');
        o.nav = $('#nav-zero');

        o.inited = true;
        return o;
    },

    onready: function() {
        var o = bmoon.zero.init();

        o.bindClick();

        o.nav && bmoon.utl.after(function() {
            o.nav.mnnav({
                ntt: mgd.ntt,
                npg: mgd.npg,
                npp: mgd.npp,
                url: '/zero'
            });
            o.getCommentNum();
        }, 'mgd.ntt !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.zero.init();

        o.submit.click(o.addImp);
    },

    getCommentNum: function() {
        var o = bmoon.zero.init();

        var ids = [];
        for (var i = 0; i < mgd.ids.length; i++) {
            ids.push('1:' + mgd.ids[i])
        }
        ids = ids.join(',');
        
        $.getJSON('/json/comment', {ids: ids}, function(data) {
            if (data.success == 1 && bmoon.utl.type(data[1]) == 'Object') {
                $.each(data[1], function(k, v) {
                    $('#comment-'+k).text(v._ntt);
                });
            }
        });
    },

    addImp: function() {
        var o = bmoon.zero.init();

        if (!$('.VAL_ADD').inputval()) return;

        var title = o.title.val(),
        content = o.content.val(),
        contact = o.contact.val(),
        post = {_op: 'add', title: title, content: content, contact: contact}
        p = $(this).parent();

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/zero', post, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                setTimeout(function() {
                    o.addoverlay.close();
                    window.location.reload();
                }, 1000)
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
        
    }
};

$(document).ready(bmoon.zero.onready);
