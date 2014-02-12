; var g_site_domain = "union.com";
var bmoon = bmoon || {};
bmoon.sora = {
    version: '1.0',

    init: function() {
        var o = bmoon.sora;

        if (o.inited) return o;
        o.inited = true;

        o.c_username = $.cookie('username'); // random user name for trace use
        o.c_mname    = $.cookie('mname');
        o.c_mnick    = $.cookie('mnick_esc');
        o.c_mmsn     = $.cookie('mmsn');
        o.c_mid      = $.cookie('mid');

        // bmoon.sora.js will used on other site
        // so, we return on other site to avoid js error
        o.inimsora = false;
        if (!jQuery('a[rel="#bd-login"]').length) return o;
        o.inimsora = true;

        o.e_content = $('#bd-content');

        o.mnick = $('#bd-mnick');
        o.member = $('#bd-member');
        o.guest = $('#bd-guest');
        o.loginhint = $('#login-hint');
        o.loginmname = $('#login-mname');
        o.loginmsn = $('#login-msn');
        o.loginoverlay = $('a[rel="#bd-login"]').overlay({
            mask: '#666', api: true,
            onLoad: function() {
                $.cookie('mmsn', null, {path: '/', domain: g_site_domain});
                o.c_mmsn  = null;
                if (o.loginmname.val().length <= 0)
                    o.loginmname.focus();
                else
                    o.loginmsn.focus();
                o.logincheckID = setInterval(o.loginCheck, 500);
            },
            onClose: function() {
                o.logincheckID && clearInterval(o.logincheckID);
            }
        });
        o.reloadAfterLogin = true;

        return o;
    },

    onready: function() {
        var o = bmoon.sora.init();

        if (!o.inimsora) return;

        o.bindClick();
        o.loginCheck();

        o.loginref = bmoon.utl.getQueryString("loginref");
        if (o.loginref) {
            o.loginoverlay.load();
        }
        o.vikierr = bmoon.utl.getQueryString("vikierr");
        if (o.vikierr) {
            $('#content').empty().append('<div class="text-error">'+o.vikierr+'</div>')
        }

        if (!o.c_username) {
            o.c_username = bmoon.utl.randomName();
            $.cookie('username', o.c_username, {'path': '/', 'expires': 36500});
        }

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.sora.init();

        $('#login-submit').click(o.login);
        $('#userlogout').click(o.logout);
        o.loginmsn.bind('keydown', 'return', o.login);
    },

    login: function() {
        var o = bmoon.sora.init();

        if (!$(".VAL_LOGIN").inputval()) return;

        var mname = o.loginmname.val(),
        msn = $.md5($.md5(o.loginmsn.val()));

        $.getJSON("/json/member/login", {mname: mname, msn: msn}, function(data) {
            if (data.success != 1 || !data.mname) {
                alert(data.errmsg || "操作失败， 请稍后再试");
                return;
            }
            o.loginCheck();
            o.reloadAfterLogin && setTimeout(function() {location.href = o.loginref || location.href;}, 1000);
        });
    },

    logout: function() {
        var o = bmoon.sora.init();

        $.getJSON('/json/member/logout');

        $.cookie('mname', null, {path: '/', domain: g_site_domain});
        $.cookie('mnick', null, {path: '/', domain: g_site_domain});
        $.cookie('mnick_esc', null, {path: '/', domain: g_site_domain});
        $.cookie('mmsn', null, {path: '/', domain: g_site_domain});
        $.cookie('mid', null, {path: '/', domain: g_site_domain});
        o.c_mnick = null;
        o.c_mname = null;
        o.c_mmsn  = null;
        o.c_mid   = null;
        o.loginmname.val("");
        o.loginCheck();
    },

    loginCheck: function() {
        var o = bmoon.sora.init();

        if (o.c_mname == null) o.c_mname = $.cookie('mname');
        if (o.c_mnick == null) o.c_mnick = $.cookie('mnick_esc');
        if (o.c_mmsn  == null) o.c_mmsn  = $.cookie('mmsn');
        if (o.c_mid   == null) o.c_mid   = $.cookie('mid');

        if (o.c_mnick != null && o.c_mmsn != null) {
            o.loginoverlay.close();
            o.mnick.text(o.c_mnick);
            o.guest.addClass('hide');
            o.member.removeClass('hide');
            o.loginmname.val(o.c_mname);
            return true;
        } else {
            o.member.addClass('hide');
            o.guest.removeClass('hide');
            return false;
        }
    }
};

$(document).ready(bmoon.sora.onready);
