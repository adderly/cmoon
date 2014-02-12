; var bmoon = bmoon || {};
bmoon.omsstat = {
    version: '1.0',

    _showTooltip: function(x, y, contents) {
        $('<div id="tooltip">' + contents + '</div>').css( {
            position: 'absolute',
            display: 'none',
            top: y + 5,
            left: x + 5,
            border: '1px solid #fdd',
            padding: '2px',
            'background-color': '#fee',
            opacity: 0.80
        }).appendTo('body').fadeIn(200);
    },
        
    init: function() {
        var o = bmoon.omsstat;
        if (o.inited) return o;

        o.sttype = $('select[name=stat-type]');
        o.stres = $('#st-res');
        o.sttable = $('#st-table');
        o.stdata = mgd.stdata;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.omsstat.init();

        o.bindClick();

        bmoon.utl.after(o.rendStat, 'mgd.stdata !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.omsstat.init();

        $('.datep').datePicker({startDate: '2010-10-10'});
        o.stres.bind('plothover', o.plotOver);
        o.sttype.change(o.rendTable);
    },

    rendStat: function() {
        var o = bmoon.omsstat.init();

        o.stdata = o.stdata || mgd.stdata;
        
        $.plot(
            o.stres,
            [
                {label: '访问量', data: o.stdata.pv},
                {label: '用户量', data: o.stdata.uv}
            ],
            {
                xaxis: {mode: 'time', timeformat: '%y-%m-%d %H:%M'},
                series: {
                    lines: {show: true},
                    points: {show:true}
                },
                grid: {hoverable: true}
            }
        );

        o.rendTable();
    },

    plotOver: function(event, pos, item) {
        var o = bmoon.omsstat.init();

        if (item) {
            if (o.previousPoint != item.datapoint) {
                o.previousPoint = item.datapoint;
                
                $('#tooltip').remove();
                
                var    x = item.datapoint[0],
                y = item.datapoint[1],
                dt = new Date(x),
                Y = dt.getFullYear(), m = dt.getMonth()+1, d = dt.getDate(),
                h = dt.getHours(),
                tm = Y+'-'+m+'-'+d+' '+h;
                
                o._showTooltip(item.pageX, item.pageY,
                               tm + ' ' + item.series.label + ' : ' + y);
            }
        } else {
            $('#tooltip').remove();
            o.previousPoint = null;            
        }
    },
    
    rendTable: function() {
        var o = bmoon.omsstat.init();

        var type = o.sttype.val();

        if (type == 'url') {
            o.rendUrl();
        } else if (type == 'refer') {
            o.rendRefer();
        } else if (type == 'area') {
            o.rendArea();
        }

        $('table', o.sttable).tablesorter({sortList: [[1,1]]});
    },

    rendUrl: function() {
        var o = bmoon.omsstat.init();
        
        o.sttable.html($(document).mntable(
            {
                _title: ['被访页面标题', function(row, col, val, tr) {
                    return $('<a target="_blank" href="'+row['href']+'">'+
                             row['title']+
                             '</a>');
                }],
                count: '访问次数'
            },
            o.stdata.url,
            {tbattr: 'class="list tablesorter"'}
        ));
    },

    rendRefer: function() {
        var o = bmoon.omsstat.init();

        o.sttable.html($(document).mntable(
            ['首次访问来路', '来路次数'], o.stdata.refer,
            {tbattr: 'class="list tablesorter"'}));
    },

    rendArea: function() {
        var o = bmoon.omsstat.init();

        o.sttable.html($(document).mntable(
            ['用户所在省份', '用户个数'], o.stdata.area,
            {tbattr: 'class="list tablesorter"'}));
    }
};

$(document).ready(bmoon.omsstat.onready);
