/***
 * APE JSF Setup
 */
APE.Config.baseUrl = 'http://js.bomdoo.com/ape'; //APE JSF 
APE.Config.domain = 'bomdoo.com'; //Your domain, must be the same than the domain in aped.conf of your server
APE.Config.server = 'push.bomdoo.com:6961'; //APE server URL

(function(){
    for (var i = 0; i < arguments.length; i++)
        APE.Config.scripts.push(APE.Config.baseUrl + '/Build/' + arguments[i] + '.js');
})('apeCore');
