; var bmoon = bmoon || {};
bmoon.index = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.index;
        if (o.inited) return o;

        o.scroll = $(".scrollable").scrollable({api: true});
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.index.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.index.init();

    }
};

$(document).ready(bmoon.index.onready);
