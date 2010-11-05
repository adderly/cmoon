//; if (!bmoon) bmoon = {};
; var bmoon = bmoon || {};
bmoon.chat = {
	version: "1.0",
	adminuser: {},
	ape: {},

	_strAction: function(type, data) {
		var
		msg = decodeURIComponent(data.msg).replace(/<\/?[^>]*>/g, ''),
		r = {
			'send':	msg,
			'msg': '����˵�� ' + msg
		};
		
		return r[type];
	},

	// {from: from, type: type, tm: tm, data: data}
	_strMsg: function(data) {
		var o = bmoon.chat.init();

		var uname = data.from == o.ape.lcsuname ? '��': data.from;
		
		return [
			'<div class="item-', data.type, '">',
			    '<span class="item-name">',	decodeURIComponent(uname),'</span>',
			    '<span class="item-time">',	data.tm,'</span>',
			    '<span class="item-content">', o._strAction(data.type, data.data),
			    '</span>',
			'</div>'
		].join('');
	},

    debug: function(msg) {
        //$('<div>'+ msg +'</div>').appendTo('body');
    },
	
	init: function(ape) {
		var o = bmoon.chat;
		if (o.inited) return o;

		var
		rmdhtml = $.browser.msie? '<bgsound id="bchat-remind"></bgsound>': '<audio id="bchat-remind"></audio>',
		html = [
			'<div id="bchat">',
				'<div id="bchat-min" title="��">&nbsp;</div>',
				'<div id="bchat-max">',
					'<div id="bchat-body">',
			    	    '<div id="bchat-hint">�ͷ���ǰ���ߣ����Թ��ܿ�����</div>',
						'<div id="bchat-close" title="��С��">&nbsp;</div>',
						'<div id="bchat-msgs" class="bchat-msgs">',
			    	        '<div class="recently"></div><div class="data"></div>',
			    	    '</div>',
						'<textarea rows="2" id="bchat-input"></textarea>',
						'<div id="bchat-hint2">Enter ������Ϣ�� ',
							'<span><input type="checkbox" id="bchat-remind-sw" checked="checked" /> ��ʾ��</span>',
						'</div>',
					'</div>',
			    	'<div id="bchat-head">',
						'<div id="bchat-trigger" title="��������">&nbsp;</div>',
						'<div id="bchat-power">&nbsp;</div>',
						'<div id="bchat-downer" title="�ر�">&nbsp;</div>',
			    	'</div>',
				'</div>',
				rmdhtml,
			'</div>'
		].join(''),
		chatbody = $('#bchat');

		o.ape = ape;
		o.inited = true;

		// application don't write bchat, append it by lcs.
		if (!chatbody.length) {
			// load css dynamic will overwrite iniUI, especially in bad network enviorment.
			//$('head').append('<link rel="stylesheet" href="http://css.kaiwuonline.com/b/chat.css" />');
			//$('head').append('<!--[if IE 6]><link rel="stylesheet" href="http://css.kaiwuonline.com/b/chat_ie6.css" /><![endif]-->');
			$('body').append(html);
		}

		o.min = $('#bchat-min');
		o.max = $('#bchat-max');
		o.trigger = $('#bchat-trigger');
		o.closer = $('#bchat-close');
		o.msglist = $('#bchat-msgs');
		o.hint = $('#bchat-hint');
		o.recentbox = $('.recently', o.msglist);
		o.databox = $('.data', o.msglist);
		//o.ape.request.send('LCS_RECENTLY', {uin: '0'});
		o.reminder = $('#bchat-remind')[0];
		o.rmdsw = $('#bchat-remind-sw');

		$('#bchat-input').bind('keydown', 'return', o.msgSend);
		// chat.css is appended by js, will overwrite my fadeIn, so, delay.
		//setTimeout(o.initUI, 2000);
		o.initUI();
		
		//$('#bchat div[title]').tooltip({position: ['top', 'left']});
		return o;
	},

	initUI: function() {
		var o = bmoon.chat.init();

		if ($.cookie('lcs_ui_max') == "0") {
			o.min.fadeIn();
		} else {
			o.max.fadeIn();
		}
		$('#bchat-downer').click(function() {
			o.max.hide();
			o.min.fadeIn();
			$.cookie('lcs_ui_max', "0");
		});
		$('#bchat-min').click(function() {
			o.min.hide();
			o.max.fadeIn();
			$.cookie('lcs_ui_max', "1");
		});
		o.closer.click(o.closeChat);
		o.trigger.click(o.openChat);
	},
	
	openChat: function() {
		var o = bmoon.chat.init();

		$('#bchat-body').fadeIn();
        $('#bchat-input').focus();
		
		o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
		o.trigger.removeClass('dirty');
		o.trigger.unbind('click').click(o.closeChat);
	},

	closeChat: function() {
		var o = bmoon.chat.init();

		$('#bchat-body').fadeOut();
		o.trigger.unbind('click').click(o.openChat);
	},

	msgSend: function() {
		var o = bmoon.chat.init(),
		mv = $('#bchat-input').val(),
		pipe = o.ape.lcsCurrentPipe,
		type = o.adminuser ? 'send': 'msg',
		html = o._strMsg({
			from: o.ape.lcsuname,
			type: type,
			tm: Date().match(/\d{1,2}:\d{1,2}:\d{1,2}/)[0],
			data: {msg: mv}
		});

		if (!mv.length) return false;
		if (mv.length > 256) {
			var ot = o.hint.html();
			o.hint.html('������Ϣ��Ҫ���� 256 ���֡�');
			setTimeout(function(){o.hint.html(ot);}, 2000);
			return false;
		}
		$('#bchat-input').val('');

		$(html).appendTo(o.databox);
		o.msglist[0].scrollTop = o.msglist[0].scrollHeight;

		if (o.adminuser.aname && pipe) {
			pipe.request.send('LCS_SEND', {msg: mv});
		} else {
			o.ape.request.send('LCS_MSG', {uname: o.ape.lcsaname, msg: mv});
		}
		
		o.soundRemind('send');
		return false;
	},

	adminOn: function(data) {
		var o = bmoon.chat.init();

 		o.debug(data.pname + ' �Ĺ���Ա ' +data.aname + ' ������');
		o.hint.html(data.aname + ' ��ǰ���ߡ�');
		o.adminuser = data;
		
		o.soundRemind('login');
	},

	adminOff: function() {
		var o = bmoon.chat.init();

 		o.debug('����Ա�߿���');
		o.hint.html('�ͷ���ǰ���ߣ����Թ��ܿ�����');
		o.adminuser = {};
	},
	
	// {from: from, type: type, tm: tm, data: data}
	onData: function(data) {
		var o = bmoon.chat.init();

		var
		uname = data.from,
		html = o._strMsg(data);

		$(html).appendTo(o.databox);
		if ($('#bchat-body').css('display') != 'none') {
			o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
		} else {
			o.trigger.addClass('dirty');
		}

		o.soundRemind('receive');
	},

	// {from: from, to: to, type: type, tm: tm, data: data}
	onRecently: function(data) {
		var o = bmoon.chat.init();

		var
		html = o._strMsg(data);
		
		$(html).prependTo(o.recentbox);
		o.msglist[0].scrollTop = o.msglist[0].scrollHeight;
	},

	soundRemind: function(type) {
		var o = bmoon.chat.init();

		if (o.rmdsw.attr('checked') == true) {
			o.reminder.src = 'http://www.kaiwuonline.com/obj/audio/'+type+'.wav';
			if (!$.browser.msie) {
				o.reminder.load();
				o.reminder.play();
			}
		}
	}
};
