$(document).ready(
function() {
	$("#aname").blur(
	function() {
		$("#hit_appnexist").fadeOut();
		$("#hit_appexist").fadeOut();

		var aname = $("#aname").val();
		if (aname.length > 0) {
			$.getJSON("/app/exist", {aname: aname},
			function(data) {
				if (data.success == 1) {
					if (data.exist == 1) {
						$("#hit_appnexist").fadeOut();
						$("#hit_appexist").fadeIn('slow');
					} else {
						$("#hit_appexist").fadeOut();
						$("#hit_appnexist").fadeIn('slow');
					}
				}
			});
		}
	});

	$("#submit").click(
	function() {
		if (!$(".VAL_NEWAPP").inputval()) {
			return;
		}

		$("#asn").attr("value", $.md5($.md5($("#asn").val())));

		var
		aname = $("#aname").val(),
		email = $("#email").val(),
		asn = $("#asn").val();


		$.getJSON("/app/new", {aname: aname, email: email, asn: asn},
		function(data) {
			if (data.success != 1 || !data.aname) {
				alert(data.errmsg || "操作失败， 请稍后再试");
				return;
			}
			$("#copy_aname").text(data.aname);
			$("#add").fadeOut();
			$("#copy").fadeIn();
			
			bmoon.lcs.loginCheck();
		});
	});
});
