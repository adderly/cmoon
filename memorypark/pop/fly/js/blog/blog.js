; var bmoon = bmoon || {};
bmoon.blog = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.blog;
        if (o.inited) return o;

        o.commentnum = 0;
        o.commentget = 0;
        o.author = $.cookie('lcs_uname');
        if (!o.author) o.author = '网友';
        o.comments = $('#comments');
        
        o.inited = true;
        return o;
    },

    // { "id": "1", "type": "0", "oid": "7", "pid": "0", "ip": "127.0.0.1", "addr": "CZ88.NET", "author": "ru_PGFOV", "content": "hey, man\n\n  Baby\n", "intime": "2010-10-22 16:53:42", "uptime": "2010-10-22 16:53:42" }
    _nodeComment: function(cmt) {
        var o = bmoon.blog.init();

        var r = $('#comment-'+cmt.id),
        html = [
            '<div class="comment-item" id="comment-', cmt.id, '">',
                '<div class="info">',
                    '<span class="author">',
                        '来自 ', cmt.addr, ' 的 ', cmt.author,
                    '</span> ',
                    '<span class="date">', cmt.intime, '</span> 说 ',
                '</div>',
                '<div class="content">',
                    '<p class="pre">', cmt.content, '</p>',
                '</div>',
            '</div>'
        ].join('');

        if (cmt.id == 0) r = [];
        if (!r.length) {
            r = $(html);
        }
        return r;
    },
    
    onready: function() {
        var o = bmoon.blog.init();

        o.bindClick();
        bmoon.utl.after(o.getComment, 'mgd.bid !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.blog.init();

        $('#submit-comment-add').click(o.addComment);
        $('#get-comment').click(o.getComment);
    },

    getComment: function() {
        var o = bmoon.blog.init();

        $.getJSON('/json/comment', {ids: '0:'+mgd.bid, nst: o.commentget}, function(data) {
            if (data.success == 1 && bmoon.utl.type(data[0]) == 'Object') {
                o.commentnum = data[0][mgd.bid].ntt;
                $('#comment-num-'+mgd.bid).text(o.commentnum);
                if (bmoon.utl.type(data[0][mgd.bid].cmts) == 'Object') {
                    $.each(data[0][mgd.bid].cmts, function(k, v){
                        if (v.pid == 0)
                            o._nodeComment(v).appendTo(o.comments);
                        else
                            o._nodeComment(v).appendTo($('#comment-'+v.pid));
                        o.commentget++;
                    });
                    if (o.commentget < o.commentnum) {
                        $('#comments-get').show();
                    } else {
                        $('#comments-get').hide();
                    }
                }
            }
        });
    },

    addComment: function() {
        var o = bmoon.blog.init();

        if (!$('.VAL_NEWCOMMENT').inputval()) return;

        var content = $('#b-comment').val(),
        p = $(this).parent();

        $(".error", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/comment',
               {
                   _op: 'add', type: 0, oid: mgd.bid, pid: 0,
                   author: o.author,
                   content: content
               },
               function(data) {
                   p.removeClass('loading');
                   
                   if (data.success == 1) {
                       p.addClass('success');
                       o.commentnum++;
                       $('#comment-num-'+mgd.bid).text(o.commentnum);
                       $('#b-comment').val('');
                       o._nodeComment({
                           id: 0,
                           addr: '火星',
                           author: '你',
                           intime: '刚刚',
                           content: content
                       }).prependTo(o.comments).fadeIn('slow');
                   } else {
                       p.addClass('error');
                       $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
                   }
               },
               'json');
        
    }
};

$(document).ready(bmoon.blog.onready);
