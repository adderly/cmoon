; var bmoon = bmoon || {};
bmoon.blogindex = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.blogindex;
        if (o.inited) return o;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.blogindex.init();

        o.bindClick();
        bmoon.utl.after(o.getCommentNum, 'mgd.ids !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.blogindex.init();

    },

    getCommentNum: function() {
        var o = bmoon.blogindex.init();

        var ids = [];

        for (var i = 0; i < mgd.ids.length; i++) {
            ids.push('0:' + mgd.ids[i])
        }
        ids = ids.join(',');
        
        $.getJSON('/json/comment', {ids: ids}, function(data) {
            if (data.success == 1 && bmoon.utl.type(data[0]) == 'Object') {
                $.each(data[0], function(k, v) {
                    $('#comment-'+k).text(v.ntt);
                });
            }
        });
    }
};

$(document).ready(bmoon.blogindex.onready);
