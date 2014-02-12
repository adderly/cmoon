; var bmoon = bmoon || {};
bmoon.okol = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.okol;
        if (o.inited) return o;
        o.inited = true;

        o.stres = $("#stat-result");
        o.previousPoint = null;
        o.stdata = mgd.stdata;

        return o;
    },
    
    onready: function() {
        var o = bmoon.okol.init();

        o.bindClick();
        //o.stdataxxx =  [ [1286769509810, 8], [1286683109810, 3], [1286596709810, 10] ];

        bmoon.utl.after(function() {
            o.stdata = mgd.stdata;
            o.flotRend(o.stdata);
        }, 'mgd.stdata !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.okol.init();

        $('.datep').datePicker({startDate: '2010-10-10'});
        $('#do').click(o.getStat);
        o.stres.bind("plothover", o.flotOver);
    },

    getStat: function() {
        var o = bmoon.okol.init();

        var type = $('#stat-type').val(),
        tms = $('#times').val(),
        tme = $('#timee').val();

        $.getJSON('/json/okol', {type: type, tms: tms, tme: tme}, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.sts) == 'Array') {
                o.stdata = [];
                $.each(data.sts, function(i, v) {
                    o.stdata.push([v.intime, v.count]);
                });
                o.flotRend(o.stdata);
            }
        });
    },

    flotRend: function(data) {
        var o = bmoon.okol.init();

        $.plot(o.stres, [data], {
            xaxis: {mode: "time", timeformat: "%y-%m-%d"},
            series: {
                lines: { show: true },
                points: { show: true }
            },
            grid: { hoverable: true, clickable: true }
        });
    },

    flotOver: function(event, pos, item) {
        var o = bmoon.okol.init();


        function showTooltip(x, y, contents) {
            $('<div id="tooltip">' + contents + '</div>').css( {
                position: 'absolute',
                display: 'none',
                top: y + 5,
                left: x + 5,
                border: '1px solid #fdd',
                padding: '2px',
                'background-color': '#fee',
                opacity: 0.80
            }).appendTo("body").fadeIn(200);
        }
        
        if (item) {
            if (o.previousPoint != item.datapoint) {
                o.previousPoint = item.datapoint;
                
                $("#tooltip").remove();
                
                var
                //x = item.datapoint[0].toFixed(2),
                //y = item.datapoint[1].toFixed(2),
                x = item.datapoint[0],
                y = item.datapoint[1],
                dt = new Date(x),
                Y = dt.getFullYear(), m = dt.getMonth()+1, d = dt.getDate(),
                tm = Y+"-"+m+"-"+d;
                
                showTooltip(item.pageX, item.pageY,
                            $('#stat-type option:selected').text() + ': ' + tm + ' = ' + y);
            }
        } else {
            $("#tooltip").remove();
            o.previousPoint = null;            
        }
    }
};

$(document).ready(bmoon.okol.onready);
