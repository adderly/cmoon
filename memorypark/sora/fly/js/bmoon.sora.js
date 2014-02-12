; var g_site_domain = "sora.com";
var bmoon = bmoon || {};
bmoon.sora = {
    version: '1.0',

    init: function() {
        var o = bmoon.sora;

        if (o.inited) return o;
        o.inited = true;

        o.content = $('#bd-content');

        return o;
    },

    onready: function() {
        var o = bmoon.sora.init();

        o.bindClick();
    },
    
    bindClick: function() {
        var o = bmoon.sora.init();
        
    }
};

$(document).ready(bmoon.sora.onready);
