﻿#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"

#ifdef HTML_PROGMEM
	#ifdef MULTI_POINTS_CONNECT
const char softm_html[] PROGMEM = R"(<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta name='theme-color' content='#abc0df'/> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <meta http-equiv='Cache-Control' content="no-cache, no-store, must-revalidate"/> <meta http-equiv='Pragma' content='no-cache'/> <title>Настройки</title> <link rel='stylesheet' type="text/css" href='global.css'> <style>input:focus{background: #FA6; outline: none;}table{width: 100%; border-collapse: collapse;}.td, .th{padding: 3px; border: 1px solid #979ab5;}.th{background: #a0a0a0; color: white; font-size: 5vw;}input, select{width: 100%; text-align: right;}tr:hover{background-color: gray; cursor: pointer;}input[type=submit]{width: auto; box-shadow: 1px 4px 8px 1px #585858;}input[type=checkbox]{width: auto;}select{width: 100%; text-align-last: right; height: auto; border: 1px solid #ccc;}@media (min-width: 640px){.th{font-size: 30px;}}</style> <script>var d=document, w=null, ob=[], fd; function setOnBlur(ip){setTimeout(function(){if (d.activeElement===ip){ip.onblur=function(){if (ip.value.length===0 || !CheckIP(ip)){setTimeout(function(){ip.focus()}, 0);}else ip.onblur=null;}}}, 0)}function CheckIP(ip){ipParts=ip.value.split("."); if (ipParts.length===4){for (i=0; i < 4; i++){TheNum=parseInt(ipParts[i]); if (TheNum >=0 && TheNum <=255){}else break;}if (i===4) return true;}return false;}function enableAuthFields(check){if (check.checked){d.getElementById('id_table_net').style.display='none';}else{d.getElementById('id_table_net').style.display='';}}function scanWifi(){w.snd('{"cmd":"scan"}'), document.getElementById("ssid").style.display='none'; var e=document.getElementById("id_lssid"); for (e.style.display="inline"; e.hasChildNodes();) e.removeChild(e.lastChild)}function setSSID(){var e=document.getElementById('id_lssid'); document.getElementById("ssid").value=e.options[e.selectedIndex].value}function listSSID(e){for (var t=document.getElementById('id_lssid'), n=0; n < e.list.length; n++){var a=parseInt(e.list[n].rssi), o=Math.min(Math.max(2 * (a + 100), 0), 100), s=document.createElement('option'); s.value=e.list[n].ssid, s.innerHTML=e.list[n].ssid + " " + o + '%', t.appendChild(s)}setSSID()}function SS(h, p, fm, fe){let tw, ws; this.snd=function(c){ws.send(c);}; this.create=function(){this.oS();}; this.close=function(){ws.close();}; this.wT=function(){clearTimeout(tw); tw=setTimeout(function(){w.close(); w.create();}, 7000);}; this.oS=function(){ws=new WebSocket(h, p); ws.onopen=function(){d.body.style.visibility='visible'; getPoints();}; ws.onclose=function(){fe();}; ws.onerror=function(){fe();}; ws.onmessage=function(e){fm(JSON.parse(e.data));}};}function getPoints(){w.snd('{"cmd":"gpoint"}');}function cL(p, a){let t="<th class='th'>ID</th><th class='th'>ИМЯ</th><th class='th'>КЛЮЧ</th>", th=d.createElement('thead'); tb=d.createElement('tbody'); tr=d.createElement('tr'); d.getElementById(p).innerHTML=''; tr.innerHTML=t; th.appendChild(tr); d.getElementById(p).appendChild(th); var id=0; a.forEach(function(o){o.id=id++; var row=d.createElement('tr'); row.innerHTML=`<td style='color: white' class='td'>${o.id}<td style='color: white' class='td'>${o.ssid}</td><td width='auto' class='td'>${o.pass}</td>`; row.addEventListener("click", function(){showD(this);}); tb.appendChild(row);}); d.getElementById(p).appendChild(tb); var table=d.getElementById(p); resizableGrid(table);}function parseCommand(e){if (e.hasOwnProperty('cmd')){switch (e.cmd){case 'gpoint': ob=e.points; cL('tw_id', ob); break; case 'cls': if (e.code===1111){alert('Лимит клиентов'); d.body.style.visibility='hidden';}w.close(); break; case 'ssl': listSSID(e); break;}}}window.onload=function(){w=new SS('ws://' + d.location.host + '/ws', ['scales'], parseCommand, function(){w.wT();}); w.oS(); fd=favDialog;}; function resizableGrid(e){var t=e.getElementsByTagName("tr")[0], n=t ? t.children : void 0; if (n){e.style.overflow="hidden"; for (var i=e.offsetHeight, o=0; o < n.length; o++){var r=s(i); n[o].appendChild(r), n[o].style.position='relative', d(r)}}function d(e){var t, n, i, o, r; e.addEventListener('mousedown', function(e){n=e.target.parentElement, i=n.nextElementSibling, t=e.pageX; var d=function(e){if ('border-box'===l(e, 'box-sizing')) return 0; var t=l(e, 'padding-left'), n=l(e, 'padding-right'); return parseInt(t) + parseInt(n)}(n); o=n.offsetWidth - d, i && (r=i.offsetWidth - d)}), e.addEventListener('mouseover', function(e){e.target.style.borderRight='2px solid #0000ff'}), e.addEventListener('mouseout', function(e){e.target.style.borderRight=''}), document.addEventListener('mousemove', function(e){if (n){var d=e.pageX - t; i && (i.style.width=r - d + 'px'), n.style.width=o + d + 'px'}}), document.addEventListener("mouseup", function(e){n=void 0, i=void 0, t=void 0, r=void 0, o=void 0})}function s(e){var t=document.createElement('div'); return t.style.top=0, t.style.right=0, t.style.width='5px', t.style.position='absolute', t.style.cursor='col-resize', t.style.userSelect='none', t.style.height=e + 'px', t}function l(e, t){return window.getComputedStyle(e, null).getPropertyValue(t)}}function closeD(){favDialog.close();}function removeD(e){var f=new FormData(e); f.append('delete', true); var r=new XMLHttpRequest(); r.onreadystatechange=function(){if (r.readyState===4){if (r.status===200){alert('Удалили!'); var u=r.responseURL; window.open(u, '_self');}else if (r.status===400){alert('Настройки не изменились');}}}; r.open('POST', '/soft.html', true); w.close(); r.send(f);}function submitD(){var f=d.getElementById('form_d'); var e=f.elements; var id=parseInt(e.id.value); if (ob[id].ssid !==e.ssid.value || ob[id].pass !==e.pass.value || ob[id].dnip !==e.dnip.value || ob[id].ip !==e.ip.value || ob[id].gate !==e.gate.value || ob[id].mask !==e.mask.value){d.getElementById('submit').type='submit'; w.close(); f.submit();}}function showD(e){var id=parseInt(e.childNodes[0].innerHTML); fd.querySelector('#submit').onclick=submitD; for (v in ob[id]){var i="#" + v; try{if (fd.querySelector(i).type==='checkbox'){fd.querySelector(i).checked=ob[id][v];}else fd.querySelector(i).value=ob[id][v];}catch (e){}}enableAuthFields(d.getElementById('dnip')); fd.showModal();}function addNet(){fd.querySelector('#submit').type='submit'; fd.querySelector("#ssid").value=''; fd.querySelector("#pass").value=''; fd.querySelector("#dnip").checked=true; enableAuthFields(d.getElementById('dnip')); fd.showModal();}</script></head><body ><dialog id='favDialog'> <form id='form_d' method='post'> <section> <input id='id' type='number' name='id' style='display: none'> <p> <span> <label for='ssid'>сеть:</label> <input id='ssid' type='text' name='ssid' placeholder='имя сети'> <select style='display:none;' id='id_lssid' onchange='setSSID();'></select> </span> </p><p> <label for='pass'>ключ:</label> <input id='pass' type='password' name='key' placeholder='пароль'> </p><p> <label for='dnip'>Динамический IP:</label> <input type='checkbox' id='dnip' name='dnip' onclick='enableAuthFields(this);'/> </p><div id='id_table_net'> <p> <label for='ip'>IP:</label> <input id='ip' type='text' name='lan_ip' onfocus='setOnBlur(this)'/> </p><p> <label for='gate'>ШЛЮЗ:</label> <input id='gate' type='text' name='gateway' onfocus='setOnBlur(this)'/> </p><p> <label for='mask'>МАСКА:</label> <input id='mask' type='text' name='subnet' onfocus='setOnBlur(this)'/> </p></div></section> <menu> <button id='remove' type='button' onclick='removeD(this.form)'>Удалить</button> <button id='closed' type='button' onclick='closeD()'>Закрыть</button> <button id='scan' type='button' onclick='scanWifi()'>Поиск</button> <button id='submit' type='button'>Сохранить</button> </menu> </form></dialog><a href='/'><img src='und.png' alt="&lt;" class='btn btn--s btn--blue'></a>&nbsp;&nbsp;<strong>НАСТРОЙКИ</strong><fieldset> <details> <summary>КОНФИГУРАЦИЯ СЕТИ</summary> <h3> <br><table id='tw_id' ></table> <br><input style='width: auto' type='button' value='ДОБАВИТЬ' onclick='addNet()'/> </h3> </details></fieldset><br/><hr><footer>2018 © Powered by www.scale.in.ua</footer></body></html>)";
	#else
		const char soft_html[] PROGMEM = R"(<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta name="theme-color" content="#abc0df"/> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/> <meta http-equiv="Pragma" content="no-cache"/> <title>Настройки</title> <link rel="stylesheet" type="text/css" href="global.css"> <style>input:focus{background: #FA6; outline: none;}table{width: 100%;}td{width: 50%;}input, select{width: 100%; text-align: right;}input[type=submit]{width: auto; box-shadow: 1px 4px 8px 1px #585858;}input[type=checkbox]{width: auto;}select{width: 100%; text-align-last: right; height: auto; border: 1px solid #ccc;}.btxt{text-decoration: none; box-shadow: 1px 4px 8px 1px #585858; padding: 3px;}</style> <script>var d=document,w=null; function setOnBlur(ip){setTimeout(function(){if (d.activeElement===ip){ip.onblur=function(){if (ip.value.length===0 || !CheckIP(ip)){setTimeout(function(){ip.focus()}, 0);}else ip.onblur=null;}}}, 0)}function CheckIP(ip){ipParts=ip.value.split("."); if (ipParts.length===4){for (i=0; i < 4; i++){TheNum=parseInt(ipParts[i]); if (TheNum >=0 && TheNum <=255){}else break;}if (i===4) return true;}return false;}function enableAuthFields(check){if (check.checked){d.getElementById('id_table_net').style.display='none';}else{d.getElementById('id_table_net').style.display='';}}function formNet(i){var f=new FormData(d.getElementById(i)); var r=new XMLHttpRequest(); r.open('POST', '/soft.html', true); r.send(f);}function SS(h, p, fm, fe){let tw, ws; this.snd=function(c){ws.send(c);}; this.create=function(){this.oS();}; this.close=function(){ws.close();}; this.wT=function(){clearTimeout(tw); tw=setTimeout(function(){w.create();}, 7000);}; this.oS=function(){ws=new WebSocket(h, p); ws.onopen=function (){d.body.style.visibility='visible'; w.snd('{"cmd":"gnet"}');}; ws.onclose=function(){fe();}; ws.onerror=function(){fe();}; ws.onmessage=function(e){fm(JSON.parse(e.data));}};}function scanWifi(){w.snd('{"cmd":"scan"}'),document.getElementById("id_stxt").innerHTML="ПОИСК...",document.getElementById("id_ssid").style.display="none"; var e=document.getElementById("id_lssid"); for (e.style.display="inline"; e.hasChildNodes();) e.removeChild(e.lastChild)}function setSSID(){var e=document.getElementById("id_lssid"); document.getElementById("id_ssid").value=e.options[e.selectedIndex].value}function listSSID(e){for (var t=document.getElementById("id_lssid"), n=0; n < e.list.length; n++){var a=parseInt(e.list[n].rssi), o=Math.min(Math.max(2 * (a + 100), 0), 100), s=document.createElement("option"); s.value=e.list[n].ssid, s.innerHTML=e.list[n].ssid + " " + o + "%", t.appendChild(s)}document.getElementById("id_stxt").innerHTML="НАЙТИ СЕТЬ:", setSSID()}function parseCommand(e){if (e.hasOwnProperty('cmd')){switch (e.cmd){case 'ssl': listSSID(e); break; case 'gnet': for (v in e){try{if (d.getElementById(v).type==='checkbox'){d.getElementById(v).checked=e[v];}else d.getElementById(v).value=e[v];}catch (e){}}enableAuthFields(document.getElementById('id_dnip')); break; case 'snet': if(e.code==200) alert("Конфигурация сохранена"); else alert("Конфигурация не изменилась"); break; case 'cls': if(e.code===1111){alert("Лимит клиентов",function (){d.getElementById('id_alr').style.display='none';},5000); d.body.style.visibility='hidden';}w.close(); break;}}}window.onload=function(){w=new SS('ws://' + d.location.host + '/ws', ['arduino'], parseCommand, function(){w.wT();}); w.oS();}; </script></head><body style='visibility: hidden'> <a href='/settings.html'><img src='und.png' alt="&lt;" class='btn btn--s btn--blue'></a>&nbsp;&nbsp;<strong>КОНФИГУРАЦИЯ СОФТ</strong> <hr> <fieldset> <h3> <form id='form_id' action='javascript:formNet("form_id")'> <br><h5 align='left'><b>Имя хоста</b></h5> <table> <tr> <td>ХОСТ:</td><td> <input id='id_host' name='host'> </td></tr></table> <br><h5 align='left'><b>Точка доступа WiFi роутера</b></h5> <table> <tr> <td><a id='id_stxt' class='btxt' href='javascript:scanWifi();'>НАЙТИ СЕТЬ:</a></td><td> <span> <input id="id_ssid" type="text" name="ssid"> <select style="display:none;" id="id_lssid" onchange="setSSID();"></select> </span> </td></tr><tr> <td>КЛЮЧ:</td><td> <input id='id_key' type='password' name='key' placeholder='пароль'> </td></tr><tr> <td>Динамический IP:</td><td> <input type='checkbox' id='id_dnip' name='dnip' onclick='enableAuthFields(this);'/> </td></tr></table> <table id='id_table_net'> <tr> <td>IP:</td><td> <input id='id_lanip' type='text' name='lanip' onfocus='setOnBlur(this)'/> </td></tr><tr> <td>ШЛЮЗ:</td><td> <input id='id_gate' type='text' name='gate' onfocus='setOnBlur(this)'/> </td></tr><tr> <td>МАСКА:</td><td> <input id='id_mask' type='text' name='mask' onfocus='setOnBlur(this)'/> </td></tr></table> <input type='submit' value='СОХРАНИТЬ'/> </form> </h3> </fieldset> <hr> <footer>2018 © Powered by www.scale.in.ua</footer></body></html>)";
	#endif // MULTI_POINTS_CONNECT	
#endif //HTML_PROGMEM


class SoftSettingsPageClass : public AsyncWebHandler {
private:
	settings_t * _value;
public:
	SoftSettingsPageClass(settings_t * value) : _value(value) {};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};	
	void handleValue(AsyncWebServerRequest * request);
};

extern SoftSettingsPageClass * SoftSettingsPage;