; var _etime = 0;
function _execAfter(func, until, sec) {
    sec = sec || 10;
    if (parseInt(_etime*(100/1000)) > sec) {
        _etime = 0;
        return;
    }
    if (eval(until)) {
        _etime = 0;
        func();
    } else {
        _etime++;
        setTimeout(function() {
            _execAfter(func, until);
        }, 100);
    }
}
/**
 * Open a connection to the specified URL, which is
 * intended to respond with an XML message.
 * 
 * @param string method The connection method; either "GET" or "POST".
 * @param string url    The URL to connect to.
 * @param string toSend The data to send to the server; must be URL encoded.
 * @param function responseHandler The function handling server response.
 */
function _xmlOpen(method, url, toSend, responseHandler)
{
    if (window.XMLHttpRequest) {
        // browser has native support for XMLHttpRequest object
        req = new XMLHttpRequest();
    } else if (window.ActiveXObject) {
        // try XMLHTTP ActiveX (Internet Explorer) version
        req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    if(req) {
        req.onreadystatechange = responseHandler;
        req.open(method, url, true);
        req.setRequestHeader("content-type","application/x-www-form-urlencoded");
        req.send(toSend);
    } else {
        //alert('Your browser does not seem to support XMLHttpRequest.');
    }
}

; var bmoon = bmoon || {};
bmoon.kol = {
    version: "1.0",

    _loadJs: function(url) {
        var e = document.createElement("script");
        e.src = url;
        e.type="text/javascript";
        document.getElementsByTagName("head")[0].appendChild(e);
    },

    // {aname: 'xxx'}
    init: function(opts) {
        var o = bmoon.kol;

        if (o.inited) return o;
        if (!opts) return {};
        if (opts.statOnly) return o;
        
        o.inited = true;

        o.aname = opts.aname || 'unknown';
        o.client = null;


        if (typeof jQuery != 'function' || jQuery.fn.jquery < '1.4.2') {
            o._loadJs('http://js.bomdoo.com/b/chatb.js');
        } else if (typeof jQuery.cookie != 'function') {
            $('head').append('<script type="text/javascript" src="http://js.bomdoo.com/b/chatc.js" charset="utf-8"></script>');
        }
        
        // wait for chatx.js loaded
        _execAfter(function() {
            // opts part one
            // can't extend all opts here, because it's timeouted lead onready's opts can't be trust
            opts = $.extend({
                css: 'http://css.bomdoo.com/b/client/lcs.css'
            }, opts || {});
            
            $('head').append('<link id="kol-lcs-css" rel="stylesheet" href="' +
                             opts.css + '" />');
            var unode = $('meta[content*="utf"]');
            if (!unode.length) unode = $('meta[content*="UTF"]');
            if (unode.length) {
                $('head').append('<script type="text/javascript" src="http://js.bomdoo.com/b/chat.js" charset="utf-8"></script>');
            } else {
                $('head').append('<script type="text/javascript" src="http://js.bomdoo.com/b/chat_gb.js" charset="gb2312"></script>');
            }
        }, "typeof jQuery == 'function' && typeof jQuery.cookie == 'function'", 10);
        
        return o;
    },

    onready: function(opts) {
        var o = bmoon.kol.init(opts);

        if (opts.statOnly) {
            _execAfter(function() {
                var sdata = 'aname=' + encodeURIComponent(opts.aname) +
                    '&url=' + encodeURIComponent(location.href) +
                    '&title=' + encodeURIComponent(document.title);
                
                _xmlOpen('POST',
                         'http://www.bomdoo.com/json/stat',
                         sdata,
                         function() {return;});
            }, "document.body != null", 10);
            
            return;
        }
        
        _execAfter(function() {
            // opts part two
            opts = $.extend({
                statOnly: false,
                aname: 'unknown',
                defaultUI: 'mid',
                restoreUI: true,
                restoreMsg: true,
                minable: false,
                hidemidOnMax: true,
                maxevent: 'click',
                pos: {
                    bottom: 0,
                    right: 0
                }
            }, opts || {});
            
            o.client = new APE.Client();
            o.client.load({
                identifier: o.aname,
                transport: 2,
                complete: function(ape) {
                    new liveCS(ape).initialize(opts);
                }
            });
        }, "typeof liveCS == 'function'", 10);
    }
};
