; var bmoon = bmoon || {};
bmoon.omsusers = {
	version: '1.0',
	
	init: function() {
		var o = bmoon.omsusers;
		if (o.inited) return o;

		o.users = $('#usertable');
		o.addoverlay = $('a[rel="#addoverlay"]').overlay({mask: '#999', api: true});

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
	},

	bindClick: function() {
		var o = bmoon.omsusers.init();

		$('#submit').unbind('click').click(o.userAdd);
		$('a.deluser').live('click', o.userDel);
	},
	
	userAdd: function() {
		var o = bmoon.omsusers.init();

		if (!$('.VAL_NEWAPP').inputval()) return;

		$('#asn').attr('value', $.md5($.md5($('#asn').val())));

		var
		aname = $('#aname').val(),
		email = $('#email').val(),
		asn = $('#asn').val();

		$.post('/oms/users', {op: 'add', aname: aname, email: email, asn: asn}, function(data) {
			if (data.success == '1') {
				o.addoverlay.close();
				o._nodeUser({aname: aname, email: email});
			} else {
				alert(data.errmsg || '操作失败， 请稍后再试');
			}
		}, 'json');
	},

	userDel: function()	{
		var o = bmoon.omsusers.init();

		var aname = $(this).attr('aname');
		
		if (confirm("确认删除 " + aname + " ?")) {
			$.post('/oms/users', {op: 'del', aname: aname}, function(data) {
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
