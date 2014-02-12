; var bmoon = bmoon || {};
bmoon.demo = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.demo;
        if (o.inited) return o;

        o.aname = $('#aname');
        o.pname = $('#plugin-name');
        o.step1 = $('#step1');
        o.step2 = $('#step2');
        o.pos = $('input.pos');
        o.ptop = $('#pos-top');
        o.pbottom = $('#pos-bottom');
        o.pleft = $('#pos-left');
        o.pright = $('#pos-right');

        o.cd_aname = $('#code-aname');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.demo.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.demo.init();

        o.aname.blur(o.appCheck);
        o.pname.change(o.changePlugin);
        o.pos.change(o.changePos);
        o.pos.bind('keydown', 'return', o.changePos);
        o.pos.bind('keydown', 'up', o.changePosVup);
        o.pos.bind('keydown', 'down', o.changePosVdown);
    },

    initClient: function(aname) {
        var o = bmoon.demo.init();

        //o.aname.unbind('blur');
        //o.aname.remove();
        o.step1.hide();
        
        $('#code-aname').text(aname);
        o.step2.fadeIn();

        bmoon.kol.onready({
            aname: aname,
            minable: true
        });
    },

    appCheck: function() {
        var o = bmoon.demo.init();

        if (!$('.VAL_ANAME').inputval()) return;

        var aname = o.aname.val(),
        p = $(this).parent();

        if (aname.length > 0) {
            $('.vres', p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            $.getJSON('/json/app/exist', {aname: aname}, function(data) {
                p.removeClass('loading');
                if (data.success == 1) {
                    if (data.exist == 1) {
                        //p.addClass('success');
                        o.initClient(aname);
                    } else {
                        p.addClass('error');
                        $('<span class="vres">' +data.msg+ '</span>').appendTo(p);
                    }
                } else {
                    p.addClass('error');
                    $('<span class="vres">' +data.errmsg+ '</span>').appendTo(p);
                }
            });

        }
    },

    changePlugin: function() {
        var o = bmoon.demo.init();

        var v = $(this).val();

        $('#code-css').html('http://css.bomdoo.com/b/client/'+v+'.css');
        $('#kol-lcs-css').attr('href', 'http://css.bomdoo.com/b/client/'+v+'.css');
    },

    changePos: function(pos, callByOther) {
        var o = bmoon.demo.init(),
        ui = bmoon.chat;

        if (!callByOther) {
            var cssk = $(this).attr('rel'),
            cssv = $(this).val();

            pos = {}; pos[cssk] = cssv;
        }

        $.each(pos, function(k, v){
            $('#code-'+k).html(v);
        });

        ui.updateUI({pos: pos});
    },

    changePosVup: function() {
        var o = bmoon.demo.init();

        var cssk = $(this).attr('rel'),
        cssv = $(this).val(),
        cssiv = parseInt(cssv) ? parseInt(cssv): 0,
        pos = {};

        if (cssv.match(/.*\%$/)) $(this).val((cssiv+1) + '%');
        else $(this).val(cssiv+1);

        pos[cssk] = $(this).val();
        
        o.changePos(pos, true);
    },
    
    changePosVdown: function() {
        var o = bmoon.demo.init();

        var cssk = $(this).attr('rel'),
        cssv = $(this).val(),
        cssiv = parseInt(cssv) ? parseInt(cssv): 0,
        pos = {};

        if (cssv.match(/.*\%$/)) $(this).val((cssiv-1) + '%');
        else $(this).val(cssiv-1);

        pos[cssk] = $(this).val();
        
        o.changePos(pos, true);
    }
};

$(document).ready(bmoon.demo.onready);
