; var g_site_domain = "bomdoo.com";
var bmoon = bmoon || {};
bmoon.lcs = {
    version: "1.0",

    init: function() {
        var o = bmoon.lcs;

        if (o.inited) return o;
        o.inited = true;

        o.aname = $('#bd-aname');
        o.member = $('#bd-member');
        o.guest = $('#bd-guest');
        o.loginaname = $('#login-aname');
        o.loginasn = $('#login-asn');
        o.loginoverlay = $('a[rel="#bd-login"]').overlay({
            mask: '#666', api: true,
            onLoad: function() {
                if (o.loginaname.val().length <= 0)
                    o.loginaname.focus();
                else
                    o.loginasn.focus();
            }
        });
        
        return o;
    },

    onready: function() {
        var o = bmoon.lcs.init();

        o.bindClick();
        o.loginCheck();

        //$('ul.sf-menu').superfish();
        
        o.loginref = bmoon.utl.getQueryString("loginref");
        if (o.loginref) {
            o.loginoverlay.load();
        }
        o.vikierr = bmoon.utl.getQueryString("vikierr");
        if (o.vikierr) {
            $('#content').empty().append('<div class="text-error">'+o.vikierr+'</div>')
        }
    },
    
    bindClick: function() {
        var o = bmoon.lcs.init();
        
        $('#login-submit').click(o.login);
        $('#userlogout').click(o.logout);
        o.loginasn.bind('keydown', 'return', o.login);
    },

    login: function() {
        var o = bmoon.lcs.init();
        
        if (!$(".VAL_LOGIN").inputval()) return;

        var
        aname = o.loginaname.val(),
        asn = $.md5($.md5(o.loginasn.val()));

        $.getJSON("/json/app/login", {aname: aname, asn: asn}, function(data) {
            if (data.success != 1 || !data.aname) {
                alert(data.errmsg || "操作失败， 请稍后再试");
                return;
            }
            o.loginoverlay.close();
            setTimeout(function() {location.href = o.loginref || location.href;}, 1000);
        });
    },
    
    logout: function() {
        var o = bmoon.lcs.init();
        
        $.getJSON('/json/app/logout');
        
        $.cookie('aname', null, {path: '/', domain: g_site_domain});
        $.cookie('aname_esc', null, {path: '/', domain: g_site_domain});
        $.cookie('asn', null, {path: '/', domain: g_site_domain});
        $.cookie('masn', null, {path: '/', domain: g_site_domain});
        o.loginaname.val("");
        o.loginCheck();
    },
    
    loginCheck: function() {
        var o = bmoon.lcs.init();
        
        var aname = $.cookie('aname_esc');
        if (aname != null) {
            o.aname.text(aname);
            o.guest.hide();
            o.member.show();
            o.loginaname.val(aname);
        } else {
            o.member.hide();
            o.guest.show();
        }
    }
};

$(document).ready(bmoon.lcs.onready);
