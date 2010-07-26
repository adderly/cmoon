; var bmoon = bmoon || {};
bmoon.omsedit = {
	version: '1.0',
	
	init: function() {
		var o = bmoon.omsedit;
		if (o.inited) return o;

		o.tabs = $('ul.css-tabs').tabs('div.css-panes > div', {api: true});

		o.inited = true;
		return o;
	},
	
	onready: function() {
		var o = bmoon.omsedit.init();

		o.bindClick();
	},

	bindClick: function() {
		var o = bmoon.omsedit.init();

		$('input.uptune').click(o.changeSet);
	},

	changeSet: function() {
		var o = bmoon.omsedit.init();

		var
		c = $(this),
		p = c.parent(),
		tune = c.attr('tune'),
		val = c.attr('checked') ? 1: 0;

		p.removeClass('success').removeClass('error').addClass('loading');

		$.post('/oms/edit', {tune: tune, tuneop: val}, function(data) {
			p.removeClass('loading');
			
			if (data.success == '1') {
				p.addClass('success');
			} else {
				p.addClass('error');
			}
		}, 'json');
	}
};

$(document).ready(bmoon.omsedit.onready);
