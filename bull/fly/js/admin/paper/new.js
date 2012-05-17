; var bmoon = bmoon || {};
bmoon.admpapernew = {
    version: '1.0',

    init: function() {
        var o = bmoon.admpapernew;
        if (o.inited) return o;

        o.e_page_class = $('#page-class');
        o.e_paper_info = $('#paper-info');
        o.e_content = $('#content');
        o.e_submit = $('#submit');

        o.e_paper_info.data('_postData', {});
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.admpapernew.init();

        o.e_content.markItUp(myMarkdownSettings);
        o.e_content.uploader({
            fileField: '#mkd-input-file',
            url: '/json/zero/image',
            dataName: 'upfile',
            extraData: {_op: 'add'},
            allowedExtension: 'jpeg, bmp, png, gif',
	        afterUpload: function (data) {
                data = jQuery.parseJSON(data);
                if (data.success == 1) {
                    o.imagenum++;
                    var s = '\n' +
                        '[![图片' + o.imagenum+ '](' + data.imageurl_zoom + ' "' + data.imagename+ '")]'+
                        '(' + data.imageurl + ' "' + '点击查看原图")' +
                        '\n';
                    $.markItUp({replaceWith: s});
                } else alert(data.errmsg || '上传图片失败');
	        },
            error: function(msg) {
                alert(msg);
            }
        });
        
        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.admpapernew.init();

        o.e_submit.click(o.savePaper);
        o.e_page_class.focus(o.inTip);
        o.e_page_class.blur(o.outTip);

        $('input, textarea', o.e_paper_info).change(function() {
            o.e_paper_info.data('_postData')[$(this).attr('name')] = $(this).val();
        });
    },

    savePaper: function() {
        var o = bmoon.admpapernew.init();

        if (!$('.VAL_NEWPAPER').inputval()) return;

        var me = $(this),
        p = me.parent(),
        pdata = o.e_paper_info.data('_postData');

        pdata._op = 'add';

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/admin/paper', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                noty({text:'文章添加成功', type: 'success', theme: 'noty_theme_mitgux'});
            } else {
                p.addClass('error');
                noty({text: data.errmsg, type: 'error', theme: 'noty_theme_mitgux'});
            }
        }, 'json');
    },

    inTip: function() {
        var o = bmoon.admpapernew.init();

        var me = o.e_page_class,
        v = me.val();

        if (!me.inputTipID) {
            me.inputTipID = setInterval(function() {
                if (me.val() != v) {
                    v = me.val();
                    o.getClassTip();
                }
            }, 500);
        }
    },

    outTip: function() {
        var o = bmoon.admpapernew.init();

        var me = o.e_page_class;

        me.inputTipID && clearInterval(me.inputTipID);
        me.inputTipID = 0;
    },

    getClassTip: function() {
        var o = bmoon.admpapernew.init();

        var me = o.e_page_class,
        v = me.val();

        $.getJSON('/json/paper/matchtitle', {title: v}, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.titles) == 'Array') {
                me.mntips(data.titles, {
                    rowspec: ['title'],
                    clickRow: function(row) {
                        o.outTip();
                        me.val(row.title);
                        o.e_paper_info.data('_postData').pid = row.id;
                        $('input[name="keyword"]').focus();
                    }
                });
            }
        });
    }
};

$(document).ready(bmoon.admpapernew.onready);