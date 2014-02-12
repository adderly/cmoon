; var bmoon = bmoon || {};
bmoon.detail = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.detail;
        if (o.inited) return o;

        o.commentnum = 0;
        o.commentget = 0;
        o.comments = $('#comments');
        o.getcomment = $('#get-comment');
        o.addcomment = $('#submit-comment-add');
        o.author = $.cookie('aname');
        if (!o.author) o.author = '网友';
        
        o.inited = true;
        return o;
    },
    
    // { "id": "1", "type": "0", "oid": "7", "pid": "0", "ip": "127.0.0.1", "addr": "CZ88.NET", "author": "ru_PGFOV", "content": "hey, man\n\n  Baby\n", "intime": "2010-10-22 16:53:42", "uptime": "2010-10-22 16:53:42" }
    _nodeComment: function(cmt) {
        var o = bmoon.detail.init();

        var r = $('#comment-'+cmt.id),
        html = [
            '<div class="comment-item" id="comment-', cmt.id, '">',
                '<div class="info">',
                    '<span class="author">',
                        cmt.author,
                    '</span> ',
                    '<span class="date">', cmt.intime, '</span> ',
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
        var o = bmoon.detail.init();

        o.bindClick();
        bmoon.utl.after(o.getComment, 'mgd.id !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.detail.init();

        o.getcomment.click(o.getComment);
        o.addcomment.click(o.addComment);
    },

    getComment: function() {
        var o = bmoon.detail.init();

        $.getJSON('/json/comment',
                  {ids: '1:'+mgd.id, _nst: o.commentget},
                  function(data) {
                      if (data.success == 1 && bmoon.utl.type(data[1]) == 'Object') {
                          o.commentnum = data[1][mgd.id]._ntt;
                          if (bmoon.utl.type(data[1][mgd.id]["cmts"]) == 'Object') {
                              $.each(data[1][mgd.id]["cmts"], function(k, v) {
                                  o.commentget++;
                                  o._nodeComment(v).appendTo(o.comments);
                              });
                              if (o.commentget < o.commentnum) o.getcomment.show();
                              else o.getcomment.hide();
                          }
                      }
                  });
    },

    addComment: function() {
        var o = bmoon.detail.init();

        if (!$('.VAL_NEWCOMMENT').inputval()) return;

        var content = $('#b-comment').val(),
        p = $(this).parent();

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/comment',
               {
                   _op: 'add', type: 1, oid: mgd.id, pid: 0,
                   author: o.author,
                   content: content
               },
               function(data) {
                   p.removeClass('loading');
                   if (data.success == 1) {
                       p.addClass('success');
                       o.commentnum++;
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
                       $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
                   }
               }, 'json');
    }
};

$(document).ready(bmoon.detail.onready);
