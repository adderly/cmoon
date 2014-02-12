; var bmoon = bmoon || {};
bmoon.omsusers = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.omsusers;
        if (o.inited) return o;

        o.users = $('#usertable');
        o.secy = $('#select-secy');
        o.addoverlay = $('a[rel="#addoverlay"]').overlay({mask: '#999', api: true});
        o.nav = $('#nav-users');

        o.inited = true;
        return o;
    },

    // {aname: 'xx', email: 'yy'}
    _nodeUser: function(data) {
        var o = bmoon.omsusers.init();

        var
        r = $('#user-' + data.aname),
        html = [
            '<tr id=user-', data.aname, '">',
                '<td>', data.aname, '</td>',
                '<td>', data.email, '</td>',
                '<td>刚刚</td>',
                '<td>刚刚</td>',
                '<td>离线</td>',
                '<td><a href="javascript: void(0);" class="deluser" aname="', data.aname, '">删除</a></td>',
            '</tr>'
        ].join('');

        if (!r.length) {
            r = $(html).appendTo(o.users);
        }
        
        return r;
    },
    
    onready: function() {
        var o = bmoon.omsusers.init();

        o.bindClick();

        bmoon.utl.after(o.rendNav, 'mgd.ntt != undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.omsusers.init();

        o.secy.change(o.setSecy);
        $('#submit').unbind('click').click(o.userAdd);
        $('a.deluser').live('click', o.userDel);
    },

    rendNav: function() {
        var o = bmoon.omsusers.init();

        o.nav.mnnav({
            ntt: mgd.ntt,
            npg: mgd.npg,
            npp: mgd.npp,
            url: '/oms/users'
        });
    },

    setSecy: function() {
        var o = bmoon.omsusers.init();

        var id = o.secy.val(),
        p = $(this).parent();
        
        $(".vres", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/oms/secy', {_op: 'mod', aname: id}, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                p.addClass('success');
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
            }
        }, 'json');
    },
    
    userAdd: function() {
        var o = bmoon.omsusers.init();

        if (!$('.VAL_NEWAPP').inputval()) return;

        $('#asn').attr('value', $.md5($.md5($('#asn').val())));

        var
        aname = $('#aname').val(),
        email = $('#email').val(),
        asn = $('#asn').val(),
        p = $(this).parent();

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/oms/users', {_op: 'add', aname: aname, email: email, asn: asn}, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                p.addClass('success');
                setTimeout(function() {
                    o.addoverlay.close();
                    o._nodeUser({aname: aname, email: email});
                }, 1000);
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
    },

    userDel: function()    {
        var o = bmoon.omsusers.init();

        var aname = $(this).attr('aname');
        
        if (confirm("确认删除 " + aname + " ?")) {
            $.post('/json/oms/users', {_op: 'del', aname: aname}, function(data) {
                if (data.success == '1') {
                    $('#user-' + aname).remove();
                } else {
                    alert(data.errmsg || '操作失败， 请稍后再试');
                }
            }, 'json');
        }
    }
};

$(document).ready(bmoon.omsusers.onready);
