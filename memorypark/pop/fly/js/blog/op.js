; var bmoon = bmoon || {};
bmoon.blogop = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.blogop;
        if (o.inited) return o;
        o.inited = true;

        $('#b-content').markItUp(myMarkdownSettings);

        return o;
    },
    
    onready: function() {
        var o = bmoon.blogop.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.blogop.init();

        $('#submit').click(o.blogOp);
        $('#submit-del').click(function() {o.blogDel(1);});
        $('#submit-rev').click(function() {o.blogDel(0);});
    },

    blogOp: function() {
        var o = bmoon.blogop.init();
        
        if (!$('.VAL_NEWBLOG').inputval()) return;

        var title = $('#title').val(),
        content = $('#b-content').val(),
        p = $(this).parent();

        $(".error", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        
        $.post('/json/blog', {_op:mgd.op? mgd.op: 'add', bid: mgd.bid, title: title, content: content}, function(data) {
            p.removeClass('loading');
            
            if (data.success == 1) {
                p.addClass('success');
                $('#title').val("");
                $('#b-content').val("");
            } else {
                p.addClass('error');
                $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
            }
        }, 'json');
    },

    blogDel: function(state) {
        var o = bmoon.blogop.init();

        // p = $(this).parent();
        // just work on $('...').click(o.blogDel)
        // don't work on $('...').click(function() {o.blogDel(x)})
        // why ?
        var p = $('#submit').parent();

        if (confirm('确认操作？')) {
            $(".error", p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            
            $.getJSON('/json/blog', {_op: 'del', bid: mgd.bid, state: state}, function(data) {
                p.removeClass('loading');
                if (data.success == 1) {
                    p.addClass('success');
                } else {
                    p.addClass('error');
                    $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
                }
            });
        }
    }
};

$(document).ready(bmoon.blogop.onready);
