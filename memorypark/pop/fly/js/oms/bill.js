; var bmoon = bmoon || {};
bmoon.omsbill = {
    version: '1.0',

    init: function() {
        var o = bmoon.omsbill;
        if (o.inited) return o;

        o.billtable = $('#bill-table');
        o.submit = $('#submit');
        o.nav = $('#nav-bill');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.omsbill.init();

        o.bindClick();

        bmoon.utl.after(o.rendNav, 'mgd.ntt !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.omsbill.init();

        $('.datep').datePicker({startDate: '2010-10-10'});
        o.submit.click(function() {
            o.turnPage(1);
        });
    },

    rendNav: function() {
        var o = bmoon.omsbill.init();

        o.nav.mnnav({
            ntt: mgd.ntt,
            npg: mgd.npg,
            npp: mgd.npp,
            cbk: o.turnPage
        });
    },

    turnPage: function(pn) {
        var o = bmoon.omsbill.init();

        var tms = $('input[name=times]').val(),
        tme = $('input[name=timee]').val();
        
        $.getJSON('/json/oms/bill', {tms: tms, tme: tme, _npg: pn}, function(data){
            if (data.success == 1) {
                $(document).mntable(
                    {
                        intime: '交易时间',
                        _type: ['交易类型', function(row, col, val, tr) {
                            var r = '';
                            if (row['btype'] == 0) r = '申请充值';
                            else if (row['btype'] == 1) r = '确认充值';
                            else if (row['btype'] == 2) r = '添加客服帐号';
                            else if (row['btype'] == 3) r = '操作回滚';
                            else r = '未知操作';
                            return $('<span>'+r+'</span>');
                        }],
                        fee: '交易金额',
                        remark: '交易备注'
                    },
                    data.bills,
                    {
                        tbattr: 'class="list"'
                    }
                ).appendTo(o.billtable.empty());
                o.nav.mnnav({
                    ntt: data._ntt,
                    npg: data._npg,
                    npp: data._npp,
                    cbk: o.turnPage
                });
            } else {
                alert(data.errmsg || '获取数据失败，请稍后再试！');
            }
        });
    }
};

$(document).ready(bmoon.omsbill.onready);

