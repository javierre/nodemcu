/*
   Uses some code from "Captive Portal" by: M. Ray Burnette 20150831
*/

/* Starts its own WiFi server to connect
     SSID StrangerThingsFrame
     password . mySTFRAME
     
    After connection, open your internet browser and go to 10.10.10.1

    27 ws2811 leds connected at pin D2
  
*/

#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include "./DNSServer.h"                  // Patched lib
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
//#include <WebSocketsClient.h>
#include <Hash.h>
#include <Wire.h>
#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <TimedAction.h>

int GlobalRed_, GlobalBlue_, GlobalGreen_;

bool repeat = false;
bool rgbcolors = false;
char A_s = 'A';
int A_int = (int)A_s;

int speed_=1;

String mssg_global = "";
String msg_all = "";

void show_color();
TimedAction timedAction = TimedAction(1000/speed_,show_color);


//ESP8266WiFiMulti WiFiMulti;
//WebSocketsClient webSocket;
const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

#define PIN D2
#define count_led 27
#define count_flash 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(count_led, PIN, NEO_RGB + NEO_KHZ800); //first number change does distance between colors

IPAddress myIP;

WebSocketsServer webSocket = WebSocketsServer(81);

int brightness_ = 125;

//WiFi
const char AP_NameChar[] = "StrangerThingsFrame";
const char WiFiAPPSK[] = "mySTFRAME";

String responseHTML;


String responseHTML_help= "<!DOCTYPE html>\n"
"<html>\n"
"  <head>\n"
"    <title>About</title>\n"
"  </head>\n"
"  <body>\n"
"  <style>\n"
"  \n"
"\n"
"\n"
"html{\n"
"  box-shadow: inset 0 -0.2em 0 0 #74CAE6;\n"
"  background-color:#F5F4ED;\n"
"  color:#525B5F;\n"
"  font-family:lato;\n"
"  font-weight:700;\n"
"  line-height:2;\n"
"  font-size:1.5em;\n"
"  min-height:10%;\n"
"}\n"
"\n"
"body{\n"
"  padding: 1em;\n"
"}\n"
"\n"
".details{\n"
"  font-weight:300;\n"
"  font-size: 0.8em;\n"
"  font-weight: normal;\n"
"  color: #808080;\n"
"}\n"
"\n"
"a{\n"
"  color:inherit;\n"
"  text-decoration:none;\n"
"  &:hover{\n"
"    text-decoration:underline;\n"
"  }\n"
"}\n"
"\n"
".contactinfo{\n"
"  margin:auto;\n"
"  width:100%;\n"
"  max-width:500px;\n"
"  min-width:260px;\n"
"  @include transition(padding-top 1s);\n"
"}\n"
"\n"
".contactinfo{\n"
"  text-align:right;\n"
"  padding-right:.25em;\n"
"}\n"
"\n"
".logo{\n"
"  height:130px;\n"
"  background-repeat: no-repeat;\n"
"  background-size:contain;\n"
"  margin-bottom:0.5em;\n"
"  background-position-y: bottom;\n"
"}\n"
"\n"
"  @media (min-height: 650px) {\n"
"    .contactinfo{\n"
"      padding-top:5em;\n"
"    }\n"
"  }\n"
"  @media (min-width: 540px) {\n"
"    html{\n"
"      font-size:2.5em;\n"
"    }\n"
"  }\n"
"  \n"
"  h1 {\n"
"    font-size: 40px;\n"
"  }\n"
"  </style>\n"
"    <div class='contactinfo'>\n"
"      <div class='logo'></div>\n"
"      <div class='name'>\n"
"        Javier P&eacute;rez\n"
"      </div>\n"
"      <div class='details'>\n"
"        <!--<div class='phone'>\n"
"           \n"
"        </div>-->\n"
"        <div class='email'>\n"
"          <a href='mailto:strangerframes@gmail.com'>\n"
"            strangerframes@gmail.com\n"
"          </a><br><br>\n"
"          <a href='www.strangerframes.com'>\n"
"            Visita Nuestra Web - See our Store\n"
"          </a><br><br>\n"
"        </div>\n"
"      </div>\n"
"<a href='/'>\n"
"            back to control\n"
"          </a><br><br><br><br> \n"
"    </div>\n"
"  </body>\n"
"</html>";

void handleRoot() {
  //webSocket.~WebSocketsServer();
  //webSocket = WebSocketsServer(81);
  webServer.send(200, "text/html", responseHTML);
}

void handleHelp() {
  webServer.send(200, "text/html", responseHTML_help);
}

void setup() {
  timedAction.disable();
  strip.begin();
  strip.setBrightness(brightness_);
  strip.show();
  strip.clear();
  strip.show();

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_NameChar, WiFiAPPSK);


  //dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.on ( "/", handleRoot );
  webServer.on ( "/about", handleHelp );
  webServer.onNotFound([]() {
    //webServer.send(200, "text/html", responseHTML);
    webServer.send ( 404, "text/plain", "not found" );
  });

  webServer.begin();
  myIP = WiFi.softAPIP();

  int status = WL_IDLE_STATUS;

  responseHTML = "<!DOCTYPE html>\n"
                 "\n"
                 "<meta charset=\"utf-8\" />\n"
                 "\n"
                 "<title>Stranger Things inspired frame</title>\n"
                 "\n"
                 "<style>\n"
                 " html, body {\n"
                 " width: 500px;\n"
                 " height: 100%;\n"
                 " margin: 0;\n"
                 " padding: 0;\n"
                 " font-family: lato;//'Pacifico', cursive;\n"                 
                 " }\n"
                 " \n"
                 ".animate\n"
                 "{\n"
                 "  transition: all 0.1s;\n"
                 " -webkit-transition: all 0.1s;\n"
                 "}\n"
                 "\n"
                 ".action-button\n"
                 "{\n"
                 " position: relative;\n"
                 " padding: 2px 10px;\n"
                 "  margin: 0px 10px 10px 0px;\n"
                 "  float: left;\n"
                 " border-radius: 10px;\n"
                 " font-family: 'Pacifico', cursive;\n"
                 " font-size: 15px;\n"
                 " color: #FFF;\n"
                 " text-decoration: none;  \n"
                 "}\n"
                 "\n"
                 ".blue\n"
                 "{\n"
                 " background-color: #3498DB;\n"
                 " border-bottom: 5px solid #2980B9;\n"
                 " text-shadow: 0px -2px #2980B9;\n"
                 " color: white;\n"
                 "}\n"
                 "\n"
                 "option:disabled{\n"
                 "   color: black;\n"
                 "   font-weight: bold;\n"
                 "}\n"
                 ".red\n"
                 "{\n"
                 " background-color: #E74C3C;\n"
                 " border-bottom: 5px solid #BD3E31;\n"
                 " text-shadow: 0px -2px #BD3E31;\n"
                 "}\n"
                 "\n"
                 ".green\n"
                 "{\n"
                 " background-color: #82BF56;\n"
                 " border-bottom: 5px solid #669644;\n"
                 " text-shadow: 0px -2px #669644;\n"
                 "}\n"
                 "\n"
                 ".yellow\n"
                 "{\n"
                 " background-color: #F2CF66;\n"
                 " border-bottom: 5px solid #D1B358;\n"
                 " text-shadow: 0px -2px #D1B358;\n"
                 "}\n"
                 "\n"
                 ".action-button:active\n"
                 "{\n"
                 " transform: translate(0px,5px);\n"
                 "  -webkit-transform: translate(0px,5px);\n"
                 " border-bottom: 1px solid;\n"
                 "}\n"
                 ".range \n"
                 "{ \n"
                 "     cursor: pointer;\n"
                 "     width: 100px !important;\n"
                 "     -webkit-appearance: none;      \n"
                 "     width:50px; \n"
                 "     border: 1px solid #e6e6e6; \n"
                 "     background-color: #e6e6e6; \n"
                 "     background-image: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#e6e6e6), to(#d2d2d2)); \n"
                 "     background-image: -webkit-linear-gradient(right, #e6e6e6, #d2d2d2); \n"
                 "     background-image: -moz-linear-gradient(right, #e6e6e6, #d2d2d2); \n"
                 "     background-image: -ms-linear-gradient(right, #e6e6e6, #d2d2d2); \n"
                 "     background-image: -o-linear-gradient(right, #e6e6e6, #d2d2d2);     \n"
                 "}\n"
                 ".range:focus\n"
                 "{\n"
                 "     border: 0 !imporant; \n"
                 "outline: none !important; \n"
                 "}\n"
                 "input[type=\"range\"].range::-webkit-slider-thumb \n"
                 "{\n"
                 "     -webkit-appearance: none; \n"
                 "     width: 10px; \n"
                 "     height: 10px; \n"
                 "background-color: #555; \n"
                 "     background-image: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#4DDBFF), to(#00CCFF)); \n"
                 "     background-image: -webkit-linear-gradient(right, #4DDBFF, #00CCFF); \n"
                 "     background-image: -moz-linear-gradient(right, #4DDBFF, #00CCFF); \n"
                 "     background-image: -ms-linear-gradient(right, #4DDBFF, #00CCFF); \n"
                 "     background-image: -o-linear-gradient(right, #4DDBFF, #00CCFF);      \n"
                 "}\n"
                 "input[type=\"range\"].round {\n"
                 "     -webkit-border-radius: 20px; \n"
                 "     -moz-border-radius: 20px; \n"
                 "     border-radius: 20px; \n"
                 "}\n"
                 "input[type=\"range\"].round::-webkit-slider-thumb {\n"
                 "     -webkit-border-radius: 5px; \n"
                 "-moz-border-radius: 5px; \n"
                 "-o-border-radius: 5px; \n"
                 "}\n"
                 "\n"
                 "\n"
                 ".vertical-lowest-first\n"
                 "{\n"
                 "     -webkit-transform:rotate(270deg); \n"
                 "     -moz-transform:rotate(270deg); \n"
                 "     -o-transform:rotate(270deg); \n"
                 "     -ms-transform:rotate(270deg); \n"
                 "     transform:rotate(270deg); \n"
                 "}\n"
                 "\n"
                 ".buttonRGB{\n"
                 "    background-color: #4CAF50; /* Green */\n"
                 "    border-style: solid;\n"
                 "    border-color: black;\n"
                 "    color: white;\n"
                 "    padding: 15px 32px;\n"
                 "    text-align: center;\n"
                 "    text-decoration: none;\n"
                 "    display: inline-block;\n"
                 "    font-size: 16px;\n"
                 "    margin: 4px 2px;\n"
                 "    cursor: pointer;\n"
                 "}\n"
                 "#xmas {\n"
                 "   background: rgb(255, 255, 255);\n"
                 "}\n"
                 "#rgbbck {\n"
                 "   background: rgb(255, 255, 255);\n"
                 "}\n"
                 "#gradrainbow {    \n"
                 "   background: rgb(255, 255, 255);\n"
                 "}\n"
                 "\n"
                 "#option-rgb{\n"
                 "  background: rgb(255, 255, 255);\n"
                 "}\n"
                 "#option-xmas{\n"
                 "  background: rgb(255, 255, 255);\n"
                 "}\n"
                 "#option-rainbow{\n"
                 "  background: rgb(255, 255, 255);\n"
                 "}\n"
                 "#option-1{\n"
                 "  background: rgb(178, 31, 53);\n"
                 "}\n"
                 "#option-2{\n"
                 "  background-color: rgb(216, 39, 53);\n"
                 "}\n"
                 "#option-3{\n"
                 "  background-color: rgb(255, 116, 53);\n"
                 "}\n"
                 "#option-4{\n"
                 "  background-color: rgb(255, 161, 53);\n"
                 "}\n"
                 "#option-5{\n"
                 "  background-color: rgb(255, 203, 53);\n"
                 "}\n"
                 "#option-6{\n"
                 "  background-color: rgb(255, 243, 53);\n"
                 "}\n"
                 "#option-7{\n"
                 "  background-color: rgb(0, 117, 58);\n"
                 "}\n"
                 "#option-8{\n"
                 "  background-color: rgb(0, 158, 71);\n"
                 "}\n"
                 "#option-9{\n"
                 "  background-color: rgb(0, 221, 53);\n"
                 "}\n"
                 "#option-10{\n"
                 "  background-color: rgb(0, 82, 165);\n"
                 "}\n"
                 "#option-11{\n"
                 "  background-color: rgb(0, 121, 231);\n"
                 "}\n"
                 "#option-12{\n"
                 "  background-color: rgb(0, 169, 252);\n"
                 "}\n"
                 "#option-13{\n"
                 "  background-color: rgb(104, 30, 126);\n"
                 "}\n"
                 "#option-14{\n"
                 "  background-color: rgb(125, 60, 181);\n"
                 "}\n"
                 "#option-15{\n"
                 "  background-color: rgb(189, 122, 246);\n"
                 "}\n"
                 "#option-w{\n"
                 "  background-color: rgb(255, 255, 255);\n"
                 "}\n"
                 "#option-off{\n"
                 "  background-color: rgb(0, 0, 0);\n"
                 "}\n"
                 "\n"
                 ".select_join {\n"
                 "  width: 170px;\n"
                 "  height: 28px;\n"
                 "  overflow: hidden;\n"
                 "  background: no-repeat right #FEFEFE;\n"
                 "  border: #000000 1px solid;\n"
                 "  -webkit-border-radius: 3px;\n"
                 "  border-radius: 3px;\n"
                 "  -webkit-box-shadow: inset 0px 0px 10px 1px #FEFEFE;\n"
                 "  box-shadow: inset 0px 0px 10px 1px #FEFEFE;\n"
                 "}\n"
                 ".select_join select {\n"
                 "  \n"
                 "  width: 170px;\n"
                 "  font-size:7pt;\n"
                 "  \n"
                 "  border: 0;\n"
                 "  border-radius: 0;\n"
                 "  height: 28px;\n"
                 "  -webkit-appearance: none;\n"
                 "}\n"
                 ".select_join select:focus {\n"
                 "  \n"
                 "}   \n"
                 "\n"
                 "</style>\n"
                 "<script language=\"javascript\" type=\"text/javascript\">\n"
                 "function sendcolorcustomize(){\n"
                 "    var posR = document.getElementById(\"RangeRed\").value;\n"
                 "    var posG = document.getElementById(\"RangeGreen\").value;\n"
                 "    var posB = document.getElementById(\"RangeBlue\").value;\n"
                 " var blink_e = document.getElementById(\"blinkselection\");\n"
                 " var blink_ = blink_e.options[blink_e.selectedIndex].value;\n"
                 " doSend('{\"op\":\"publish\", \"topic\":\"/color\", \"msg\": {\"R\":\"'+posR+'\", \"G\":\"'+posG+'\", \"B\":\"'+posB+'\", \"Blink\":\"'+blink_+'\"}}');\n"
                 "}\n"
                 "function getvalslider(){\n"
                 "    var posR = document.getElementById(\"RangeRed\").value;\n"
                 "    var posG = document.getElementById(\"RangeGreen\").value;\n"
                 "    var posB = document.getElementById(\"RangeBlue\").value;\n"
                 "    var butt_ = document.getElementById(\"buttonRGB\");\n"
                 "    butt_.style.background=\"rgb(\"+posR+\", \"+posG+\", \"+posB+\")\";\n"
                 "    butt_.style.color=\"white\";\n"
                 "    if(parseInt(posR)+parseInt(posG)+parseInt(posB)>200) butt_.style.color=\"black\";\n"
                 "}\n"
                 "var url;\n"
                 "\n"
                 "function change_speedlevel() {\n"
                 " var speed_=document.getElementById(\"speedlevel\").value;\n"                 
                 " doSend('{\"op\":\"publish\", \"topic\":\"/speedlevel\", \"msg\": {\"speedlevel\":\"'+speed_+'\"}}');\n"
                 "}\n"
                 "\n"                 
                 "function change_brightness() {\n"
                 "    var e = document.getElementById(\"brightnesslevel\");\n"
                 " var strUser = e.options[e.selectedIndex].value;   \n"
                 " doSend('{\"op\":\"publish\", \"topic\":\"/brightnesslevel\", \"msg\": {\"brightnesslevel\":\"'+strUser+'\"}}');\n"
                 "}\n"
                 "\n"
                 "function change_color() {\n"
                 "    //var color = document.body.colorPick.select.selectedIndex;\n"
                 "    var e = document.getElementById(\"colorPick\");\n"
                 " var strUser = e.options[e.selectedIndex].value;   \n"
                 " if((e.selectedIndex>=1 && e.selectedIndex<=6) || e.selectedIndex==24) e.style.background = \"white\";\n"
                 " else e.style.background = e.options[e.selectedIndex].value; //document.getElementById(myid).style.backgroundColor\n"
                 " //e.options[e.selectedIndex].value;    \n"
                 " var rgb_ = e.options[e.selectedIndex].value.match(/^rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)$/);\n"
                 " var red_=parseInt(rgb_[1]);\n"
                 " var green_=parseInt(rgb_[2]);\n"
                 " var blue_=parseInt(rgb_[3]);\n"
                 " var blink_e = document.getElementById(\"blinkselection\");\n"
                 " var blink_ = blink_e.options[blink_e.selectedIndex].value;\n"
                 " if(red_==1 && green_==2 && blue_ == 3)  doSend('{\"op\":\"publish\", \"topic\":\"/stranger_message\", \"msg\": {\"loop\":true, \"rgb\":\" \", \"text\":\" \", \"all\":\"rainbow\"}}');\n"
                 " else  doSend('{\"op\":\"publish\", \"topic\":\"/color\", \"msg\": {\"R\":\"'+red_+'\", \"G\":\"'+green_+'\", \"B\":\"'+blue_+'\", \"Blink\":\"'+blink_+'\"}}');\n"
                 "}\n"
                 "function sleep(milliseconds) {\n"
                 "console.log(\"wait---\");\n"
                 "  var start = new Date().getTime();\n"
                 "  for (var i = 0; i < 1e7; i++) {\n"
                 "    if ((new Date().getTime() - start) > milliseconds){\n"
                 "      break;\n"
                 "    }\n"
                 "  }\n"
                 "}\n"
                 "\n"
                 "  function init()\n"
                 "  {\n"
                 "    var ip_serv = location.hostname;\n"
                 " // alert(\"ws://\"+ip_serv+\":8000/\");\n"
                 " url  = \"ws://";//document.myform.url.value = \"ws://";
  responseHTML += myIP.toString(); //myIP;
  //"10.10.10.1:81/\";\n"
  responseHTML += ":81/\";\n"
                  " document.myform.inputtext.value = \"Run!\";\n"
                  " document.myform.disconnectButton.disabled = true;\n"
                  "\n"
                  "\n"
                  "  }\n"
                  "\n"
                  "  function Start_advertise()\n"
                  "  {\n"
                  " doSend('{\"op\":\"advertise\", \"topic\":\"/stranger_message\"}');\n"
                  "  }\n"
                  "\n"
                  "\n"
                  "  function doConnect()\n"
                  "  {\n"
                  "    websocket = new WebSocket(url);//document.myform.url.value);\n"
                  "    websocket.onopen = function(evt) { onOpen(evt) };\n"
                  "    websocket.onclose = function(evt) { onClose(evt) };\n"
                  "    websocket.onmessage = function(evt) { onMessage(evt) };\n"
                  "    websocket.onerror = function(evt) { onError(evt) };\n"
                  "    \n"
                  "    \n"
                  "  }\n"
                  "\n"
                  "\n"
                  "  function onOpen(evt)\n"
                  "  {\n"
                  "    writeToScreen(\"connected\");\n"
                  "    Start_advertise();   \n"
                  "    document.myform.connectButton.disabled = true;\n"
                  "    document.myform.disconnectButton.disabled = false; \n"
                  "  }\n"
                  "\n"
                  "  function onClose(evt)\n"
                  "  {\n"
                  "   websocket = null;\n"
                  "    writeToScreen(\"disconnected\");\n"
                  " document.myform.connectButton.disabled = false;\n"
                  " document.myform.disconnectButton.disabled = true;\n"
                  "  sleep(2000);\n"
                  "  doConnect();\n"
                  "  }\n"
                  "\n"
                  "  function onMessage(evt)\n"
                  "  {\n"
                  "    writeToScreen(\"response: \" + evt.data);\n"
                  "  }\n"
                  "\n"
                  "  function onError(evt)\n"
                  "  {\n"
                  "    writeToScreen('error: ' + evt.data);\n"
                  "    try{\n"
                  "     }catch(err) {\n"
                  "    }\n"
                  "  websocket = null;\n"
                  "  document.myform.connectButton.disabled = false;\n"
                  "  document.myform.disconnectButton.disabled = true;\n"
                  "  sleep(2000);\n"
                  "  websocket.close();\n"
                  "\n"
                  " document.myform.connectButton.disabled = false;\n"
                  " document.myform.disconnectButton.disabled = true;\n"
                  "  }\n"
                  "\n"
                  "  function doSend(message)\n"
                  "  {\n"
                  "    writeToScreen(\"sent: \" + message); \n"
                  "    websocket.send(message);\n"
                  "  }\n"
                  "\n"
                  "  function writeToScreen(message)\n"
                  "  {\n"
                  "    //document.myform.outputtext.value += message\n"
                  " //document.myform.outputtext.scrollTop = document.myform.outputtext.scrollHeight;\n"
                  "\n"
                  "  }\n"
                  "\n"
                  "  window.addEventListener(\"load\", init, false);\n"
                  "\n"
                  "\n"
                  "   function All() {\n"
                  " var rgbit_=document.myform.rgbit.checked;\n"
                  " doSend('{\"op\":\"publish\", \"topic\":\"/stranger_message\", \"msg\": {\"loop\":true, \"rgb\":'+rgbit_+', \"text\":\" \", \"all\":\"all\"}}');\n"
                  "   }\n"
                  "\n"
                  "   function Blink() {\n"
                  " var rgbit_=document.myform.rgbit.checked;\n"
                  " doSend('{\"op\":\"publish\", \"topic\":\"/stranger_message\", \"msg\": {\"loop\":true, \"rgb\":'+rgbit_+', \"text\":\" \", \"all\":\"blink\"}}');\n"
                  "   }\n"
                  "\n"
                  "   function Customize() {\n"
                  "     var tab_c=document.getElementById(\"table_custo\");\n"
                  "     if(tab_c.style.display==\"none\")     tab_c.style.display=\"block\";\n"
                  "     else tab_c.style.display=\"none\";\n"
                  "}\n"
                  "\n"
                  "   function Rainbow() {\n"
                  " var rgbit_=document.myform.rgbit.checked;\n"
                  " doSend('{\"op\":\"publish\", \"topic\":\"/stranger_message\", \"msg\": {\"loop\":true, \"rgb\":'+rgbit_+', \"text\":\" \", \"all\":\"rainbow\"}}');\n"
                  "   }\n"
                  "\n"
                  "   function sendText() {\n"
                  " var txt_=document.myform.inputtext.value;\n"
                  " var loopit_=document.myform.loopit.checked;\n"
                  " var rgbit_=document.myform.rgbit.checked;\n"
                  " var speed_=document.getElementById(\"speedlevel\").value;\n"                  
                  " doSend('{\"op\":\"publish\", \"topic\":\"/stranger_message\", \"msg\": {\"loop\":'+loopit_+', \"rgb\":'+rgbit_+', \"all\":\" \", \"speed\":\"'+speed_+'\", \"text\":\"'+txt_+'\"}}');\n"
                  "   }\n"
                  "\n"
                  "   \n"
                  "  function clearText() {\n"
                  "   //document.myform.outputtext.value = \"\";\n"
                  "   }\n"
 "\n"
 "   function doDisconnect() {\n"
 "   websocket.close();\n"
 "   }\n"
 "\n"
 "\n"
 "</script>\n"
 "\n"
 "<div id=\"output\"></div>\n"
 "\n"
 "<form name=\"myform\">\n"
 "<p>\n"
 "</p>\n"
 "<p>\n"
 "<h1 align=\"center\">STRANGER THINGS INSPIRED DECORATION FRAME</h1>\n"
 "<br><br>\n"
 "<center><table style=\"width:60%\" align=\"center\">\n"
 "<tr><th colspan=\"3\"></th><th align=\"right\" colspan=\"1\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <input type=\"button\" name=connectButton value=\"Connect\" onClick=\"doConnect();\" class=\"action-button shadow animate blue\"><input type=\"button\" name=disconnectButton value=\"Disconnect\" onClick=\"doDisconnect();\" class=\"action-button shadow animate blue\"></th></tr>\n"
 "</table></center>\n"
 "<table style=\"width:100%\" align=\"center\">\n"
 "  <tr><th>Message to send: <input type=\"checkbox\" name=\"loopit\" value=\"loop_\"> Loop  <input type=\"checkbox\" name=\"rgbit\" value=\"rgb_\">RGB&nbsp;&nbsp;</th>\n"
 "  <th><br><br><input type=\"button\" class=\"action-button shadow animate blue\" name=sendButton value=\"Send\" onClick=\"sendText();\"><select id=\"speedlevel\" name=\"speedlevel\" class=\"action-button shadow animate blue\" onchange=\"change_speedlevel();\">\n"  
 "  <option id=\"0\" disabled=\"disabled\" value=\"1\" selected>Speed</option>\n"
 "  <option id=\"1s\" value=\"1\">1</option>\n"
 "  <option id=\"2s\" value=\"2\">2</option>\n"
 "  <option id=\"3s\" value=\"3\">3</option>\n"
 "  <option id=\"4s\" value=\"4\">4</option>\n"
 "</select><br><br><br></th></tr>\n"
 "</table>\n"
 "<center><textarea name=\"inputtext\" cols=\"50\"></textarea></center>\n"
 "</p>\n"
 "<p>\n"
 "<br>\n"
 "</p>\n"
 "<p>\n"
 "<select id=\"blinkselection\" name=\"blinkselection\" onchange=\"change_color();\" class=\"action-button shadow animate blue\">\n"
 "  <option id=\"blink\" value=\"blink\">Blink</option>\n"
 "  <option id=\"fade\" value=\"fade\">Fade</option>\n"
 "  <option id=\"fixed\" value=\"fixed\">Fixed</option>\n"
 "</select>\n"
 "<select id=\"brightnesslevel\" name=\"brightnesslevel\" onchange=\"change_brightness();\" class=\"action-button shadow animate blue\">\n"  
 "  <option id=\"255l\" disabled=\"disabled\" value=\"Bright_\" selected>Brightness</option>\n"
 "  <option id=\"255l\" value=\"255\">255</option>\n"
 "  <option id=\"100l\" value=\"100\">100</option>\n"
 "  <option id=\"50l\" value=\"50\">50</option>\n"
 "  <option id=\"25l\" value=\"25\">25</option>\n"
 "</select>\n"
 "<input type=\"button\" name=sendButton value=\"Customize\" onClick=\"Customize();\" class=\"action-button shadow animate blue\">\n"
 "<select id=\"colorPick\" name=\"colorPick\" class=\"select_join\" onchange=\"change_color();\" class=\"action-button shadow animate blue\">\n"
 "  <option disabled=\"disabled\" selected>Select Color</option>\n"
 "  <option id=\"gradrainbow\" value=\"rgb(1, 2, 4)\">static Rainbow</option>\n"
 "  <option id=\"gradrainbow\" value=\"rgb(1, 2, 3)\">Rainbow</option>\n"
 "  <option id=\"gradrainbow\" value=\"rgb(3, 3, 3)\">Random</option>\n"
 "  <option id=\"gradrainbow\" value=\"rgb(4, 4, 4)\">Madness</option>\n"
 "  <option id=\"rgbbck\" value=\"rgb(1, 0, 0)\">RGB</option>\n"
 "  <option id=\"xmas\" value=\"rgb(1, 1, 1)\">Xmas</option>\n"
 "  <option id=\"option-1\" value=\"rgb(178, 31, 53)\">Red1</option>\n"
 "  <option id=\"option-2\" value=\"rgb(216, 39, 53)\">Red2</option>\n"
 "  <option id=\"option-3\" value=\"rgb(255, 116, 53)\">Orange1</option>\n"
 "  <option id=\"option-4\" value=\"rgb(255, 161, 53)\">Orange2</option>\n"
 "  <option id=\"option-5\" value=\"rgb(255, 203, 53)\">Yellow1</option>\n"
 "  <option id=\"option-6\" value=\"rgb(255, 243, 53)\">Yellow2</option>\n"
 "  <option id=\"option-7\" value=\"rgb(0, 117, 58)\">Green1</option>\n"
 "  <option id=\"option-8\" value=\"rgb(0, 158, 71)\">Green2</option>\n"
 "  <option id=\"option-9\" value=\"rgb(0, 221, 53)\">Green3</option>\n"
 "  <option id=\"option-10\" value=\"rgb(0, 82, 165)\">Blue1</option>\n"
 "  <option id=\"option-11\" value=\"rgb(0, 121, 231)\">Blue2</option>\n"
 "  <option id=\"option-12\" value=\"rgb(0, 169, 252)\">Blue3</option>\n"
 "  <option id=\"option-13\" value=\"rgb(104, 30, 126)\">Violet1</option>\n"
 "  <option id=\"option-14\" value=\"rgb(125, 60, 181)\">Violet2</option>\n"
 "  <option id=\"option-15\" value=\"rgb(189, 122, 246)\">Violet3</option>\n"
 "  <option id=\"option-w\" value=\"rgb(255, 255, 255)\">White</option>\n"
 "  <option id=\"option-off\" value=\"rgb(0, 0, 0)\">Off</option>\n"
 "  <option id=\"gradrainbow\" value=\"rgb(5, 5, 5)\">RUN</option>\n"
 "</select></td>\n"
 "<input type=\"hidden\" name=\"snd_cmd\" value=\"\">\n"
                  "</p>\n"
                  "\n"
                  "\n"
                  "</form>\n"
                  "<div id=\"table_custo\" style=\"display: none;\"><table id=\"table_customize\"><td>\n"
                  "<div class=\"container\" style=\"margin-left: 0px;\">\n"
                  "\n"
                  "<p align=\"center\"> Red: <br>255</p>        \n"
                  "       <br/><br/>\n"
                  "\n"
                  "<input id=\"RangeRed\" type=\"range\" min=\"0\" max=\"255\" step=\"-5\" value=\"50\" class=\"range vertical-lowest-first\" onchange=\"getvalslider();\" onclick=\"getvalslider();\" onmousemove=\"getvalslider();\"/>\n"
                  "<br/><br/><br><p align=\"center\">0</p>\n"
                  "</div>\n"
                  "\n"
                  "</td><td>\n"
                  "<div class=\"container\" style=\"margin-left: 0px;\">\n"
                  "\n"
                  "<p align=\"center\"> Green: <br>255</p>        \n"
                  "       <br/><br/>\n"
                  "\n"
                  "<input id=\"RangeGreen\" type=\"range\" min=\"0\" max=\"255\" step=\"-5\" value=\"50\" class=\"range vertical-lowest-first\" onchange=\"getvalslider();\" onclick=\"getvalslider();\" onmousemove=\"getvalslider();\"/>\n"
                  "<br/><br/><br><p align=\"center\">0</p>\n"
                  "</div>\n"
                  "</td>\n"
                  "\n"
                  "<td>\n"
                  "<div class=\"container\" style=\"margin-left: 0px;\">\n"
                  "\n"
                  "<p align=\"center\"> Blue: <br>255</p>       \n"
                  "       <br/><br/>\n"
                  "\n"
                  "<input id=\"RangeBlue\" type=\"range\" min=\"0\" max=\"255\" step=\"-5\" value=\"50\" class=\"range vertical-lowest-first\" onchange=\"getvalslider();\" onclick=\"getvalslider();\" onmousemove=\"getvalslider();\"/>\n"
                  "<br/><br/><br><p align=\"center\">0</p>\n"
                  "</div>\n"
                  "</td><td><input type=\"submit\" value=\"Send\" class=\"buttonRGB\" id=\"buttonRGB\" onclick=\"sendcolorcustomize();\"></td>\n"
                  "</table></div>\n"
                  "<input type=\"button\" name=connectButton value=\"About\" onClick=\"location.href='/about';\" class=\"action-button shadow animate blue\">\n"
                  "</html> ";

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.printf("WiFi connected, my IP is ");
  Serial.println(WiFi.localIP());


}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void loop() {
  //dnsServer.processNextRequest();
  webServer.handleClient();
  //if(webSocket.isConnected()) webSocket.loop();
  //if(webSocket.connectionCount()>0) webSocket.loop();
  webSocket.loop();
  timedAction.check();
}




void show_color() { //String mssg){
  String mssg = mssg_global;
  timedAction.disable();
  //Serial.println("rec__: ");
  //Serial.println( mssg.length() ); 
  strip.clear();
  strip.show();
  strip.setBrightness(brightness_);
  int line = 0;
  mssg.toUpperCase();
  for (int im = 0; im < mssg.length(); im++) {
    //Serial.println( mssg );
    
    //Serial.println( mssg );
    //Serial.println( im );
    char mssg_letter = mssg.charAt(im);
    //Serial.println( mssg_letter );
    int mssg_int = (int) mssg_letter - A_int; //+11;//atoi(&mssg_letter)-A_int;
    if (mssg_int >= 8 && mssg_int <= 16) mssg_int = 16 - (mssg_int - 8);
    //if(mssg_int>27)mssg_int=mssg_int+3;
    //Serial.println( mssg_int );

    int mssg_int2 = 25 - mssg_int;


    //if(mssg_int >=11 && mssg_int < 40){
    if (mssg_int >= 0 && mssg_int < 27) {     
      if (rgbcolors) {        
        if ((mssg_int % 3) == 0) strip.setPixelColor(mssg_int2, strip.Color(0, 255, 0));
        else if ((mssg_int % 2) == 0) strip.setPixelColor(mssg_int2, strip.Color(255, 0, 0));
        else strip.setPixelColor(mssg_int2, strip.Color(0, 0, 255));
      }
      else {        
        if (mssg_int == 0 || mssg_int == 5 || mssg_int == 12 || mssg_int == 18 || mssg_int == 19 || mssg_int == 23) strip.setPixelColor(mssg_int2, strip.Color(255, 255, 0)); //amarillo
        else if (mssg_int == 1 || mssg_int ==  4 || mssg_int ==  7 || mssg_int == 16 || mssg_int == 20) strip.setPixelColor(mssg_int2, strip.Color(51, 51, 255)); //azul
        else if (mssg_int == 2 || mssg_int == 6 || mssg_int == 15 || mssg_int == 10 || mssg_int == 11 || mssg_int == 8 || mssg_int == 21 || mssg_int == 24 || mssg_int == 25) strip.setPixelColor(mssg_int2, strip.Color(255, 51, 255)); //violeta
        else strip.setPixelColor(mssg_int2, strip.Color(102, 255, 255)); //celeste
        //if(mssg_int== 1 || mssg_int==  4 || mssg_int==  7 || mssg_int==  8 || mssg_int== 20) strip.setPixelColor(mssg_int, strip.Color(255, 102, 255)); //celeste
      }     
      delay(5);
      strip.show();           
      delay(1000/speed_);
    }

    strip.setPixelColor(mssg_int2, strip.Color(0, 0, 0));
    strip.show();
    delay(500/speed_);
  }

  strip.show();
  delay(1000/speed_);
  //if(repeat) show_color(mssg);
  if (repeat) {
    timedAction = TimedAction(1000/speed_, show_color);
    timedAction.enable();
  }

}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  //void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {
  //repeat = false;
  switch (type) {
    case WStype_DISCONNECTED:
      //webSocket.~WebSocketsServer();
      strip.clear();
      break;
    case WStype_CONNECTED: {
        repeat = false;
        timedAction.disable();
        Serial.println("Connected!!!");
        for (int w = 0; w < count_flash; w++) {
          for (int j = 0; j < count_led; j++) {
            strip.setPixelColor(j, strip.Color(255, 0, 0));
          }
          strip.show();
          delay(500/speed_);
          strip.clear();
          strip.show();
          delay(500/speed_);
        }
        //webSocket.sendTXT("{\"op\": \"subscribe\", \"topic\": \"/stranger_message\"}");
        //webSocket.sendTXT(num, "{\"op\": \"subscribe\", \"topic\": \"/stranger_message\"}", lenght);

        break;
      }
    case WStype_TEXT: {
        repeat = false;
        Serial.printf("[WSc] get text: %s\n", payload);
        String txt_ = "{\"from\": \"server\", \"command\": \"Connected to server\"}";
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject((char*)payload);

        String op_ = root["op"];
        String topic_ = root["topic"];

        if (topic_ == "/speedlevel" && op_ == "publish") {                   
          String speedo = root["msg"]["speedlevel"];
          speed_ = speedo.toInt();          
        }
        else if (topic_ == "/brightnesslevel" && op_ == "publish") {
          String Bright_ = root["msg"]["brightnesslevel"];
          brightness_ = Bright_.toInt();
          strip.setBrightness(brightness_);
          strip.show();
          Serial.printf("%d\n",  brightness_);
        }
        else {

          repeat = false;
          timedAction.disable();
          strip.clear();
          strip.show();
          strip.setBrightness(brightness_);

          if (topic_ == "/color" && op_ == "publish") {

            String Red_ = root["msg"]["R"];
            String Green_ = root["msg"]["G"];
            String Blue_ = root["msg"]["B"];
            String Blink_ = root["msg"]["Blink"];

            if (Red_.toInt() == 1 && Green_.toInt() == 2 && Blue_.toInt() == 4) { //static Rainbow
              Serial.printf("STRB");
              timedAction.disable();
              strip.setBrightness(brightness_);
              strip.show();
              delay(500/speed_);
              if (Blink_ == "blink") {
                //blinkrainbow();
                timedAction = TimedAction(1000/speed_,blinkrainbow);
                timedAction.enable();

              }
              else if (Blink_ == "fixed") {
                fixedrainbow();
                strip.show();

              }
              else if (Blink_ == "fade") {
                //faderainbow();
                timedAction = TimedAction(1000/speed_,faderainbow);
                timedAction.enable();

              }
            }
            else if (Red_.toInt() == 5 && Green_.toInt() == 5 && Blue_.toInt() == 5) { //RUN
               RUN();
            }
            else if (Red_.toInt() == 4 && Green_.toInt() == 4 && Blue_.toInt() == 4) { //madness
               madness();
            }
            else if (Red_.toInt() == 3 && Green_.toInt() == 3 && Blue_.toInt() == 3) { //random
              timedAction.disable();
              strip.clear();
              strip.show();
              strip.setBrightness(brightness_);
              for (int j = 0; j < count_led; j++) {
                int r_, g_, b_;
                do{
                   r_=random(0,255);
                   g_=random(0,255);
                   b_=random(0,255);
                }while(r_+g_+b_<50);
                strip.setPixelColor(j, r_, g_, b_);//strip.Color(random(0, 255), random(0, 255), random(0, 255)));                
              }
              strip.setBrightness(brightness_);
              strip.show();
              //strip.show();
              if (Blink_ == "blink") {
                //blinkrainbow();
                timedAction = TimedAction(1000/speed_,blinkrandom);
                timedAction.enable();

              }
              else if (Blink_ == "fixed") {

                strip.show();
              }
              else if (Blink_ == "fade") {
                //faderainbow();
                timedAction = TimedAction(1000/speed_,faderandom);
                timedAction.enable();

              }

            }
            else if (Red_.toInt() == 1 && Green_.toInt() == 0 && Blue_.toInt() == 0) { //RGB
              //RGB
              rgbcolors = true;
              all();
              if (Blink_ == "blink") {
                strip.setBrightness(brightness_);
                timedAction.disable();
                //delay(1000);
                //strip.clear();
                //strip.setBrightness(0);
                //strip.show();
                //delay(1000);
                timedAction = TimedAction(2000, blinkpurergb);
                timedAction.enable();
              }
              else if (Blink_ == "fixed") {

              }
              else if (Blink_ == "fade") {
                timedAction.disable();
                strip.setBrightness(brightness_);
                for (int jj = 1; jj <= 10; jj++) {
                  //strip.setBrightness(100-jj*10);
                  strip.setBrightness(brightness_ - jj * brightness_ / 10);
                  strip.show();
                  delay(100/speed_);
                }
                delay(500/speed_); //recheck
                timedAction = TimedAction(3000/speed_, fadexmas);
                timedAction.enable();
              }
            }
            else if (Red_.toInt() == 1 && Green_.toInt() == 1 && Blue_.toInt() == 1) {
              //XMAS
              rgbcolors = false;
              all();
              if (Blink_ == "blink") {
                timedAction.disable();
                delay(1000/speed_);
                strip.clear();
                //strip.setBrightness(0);
                strip.show();
                delay(1000/speed_);
                timedAction = TimedAction(2000, blinkxmas);
                timedAction.enable();
              }
              else if (Blink_ == "fixed") {

              }
              else if (Blink_ == "fade") {
                timedAction.disable();
                strip.setBrightness(brightness_);
                for (int jj = 1; jj <= 10; jj++) {
                  //strip.setBrightness(100-jj*10);
                  strip.setBrightness(brightness_ - jj * brightness_ / 10);
                  strip.show();
                  delay(100/speed_);//recheck
                }
                delay(500/speed_);
                timedAction = TimedAction(3000/speed_, fadexmas);
                timedAction.enable();
              }
            }
            else if (Red_.toInt() == 1 && Green_.toInt() == 2 && Blue_.toInt() == 3) {
              //RAINBOW
              Serial.printf("RAINBOW");
              timedAction.disable();
              strip.setBrightness(brightness_);
              strip.show();
              delay(500/speed_);//recheck
              rainbow();
              strip.show();
            }
            else {
              for (int j = 0; j < count_led; j++) {
                strip.setPixelColor(j, strip.Color(Red_.toInt(), Green_.toInt(), Blue_.toInt()));                
              }
              strip.show();
              if (Blink_ == "blink") {
                GlobalRed_ = Red_.toInt();
                GlobalGreen_ = Green_.toInt();
                GlobalBlue_ = Blue_.toInt();
                timedAction.disable();
                //blinkrgb();
                delay(1000/speed_);
                strip.clear();
                strip.show();
                delay(1000/speed_);
                timedAction = TimedAction(2000/speed_, blinkrgb);
                timedAction.enable();
              }
              else if (Blink_ == "fixed") {

              }
              else if (Blink_ == "fade") {
                GlobalRed_ = Red_.toInt();
                GlobalGreen_ = Green_.toInt();
                GlobalBlue_ = Blue_.toInt();
                timedAction.disable();
                strip.setBrightness(brightness_);
                //blinkrgb();
                //delay(1000);
                for (int jj = 1; jj <= 10; jj++) {
                  //strip.setBrightness(100-jj*10);
                  strip.setBrightness(brightness_ - jj * brightness_ / 10);
                  strip.show();
                  delay(100/speed_);//recheck
                }
                //strip.clear();
                //strip.show();
                delay(500/speed_);//recheck
                timedAction = TimedAction(3000/speed_, fadergb);
                timedAction.enable();

              }
            }

            strip.show();
          } else {
            String msg_txt = root["msg"]["text"];
            String msg_all2 = root["msg"]["all"];

            msg_all = msg_all2;
            repeat = root["msg"]["loop"];
            rgbcolors = root["msg"]["rgb"];

            Serial.print("----------topic: ");
            Serial.println(topic_);
            Serial.println(op_);
            /* if(cmd_ == "on"){

              }*/



            if (topic_ == "/stranger_message" && op_ == "publish") {              
              mssg_global = msg_txt;
              if (msg_all == " ") {                
                String speedo = root["msg"]["speed"];
                speed_ = speedo.toInt();
                show_color(); //msg_txt);
              }
              else if (msg_all == "rainbow") rainbow();
              else if (msg_all == "blink") blinkl();
              else if (msg_all == "all") all();
            }

          }
        }
        break;
      }
    case WStype_BIN:
      Serial.printf("[WSc] get binary lenght: %u\n", lenght);
      hexdump(payload, lenght);

      // send data to server
      // webSocket.sendBIN(payload, lenght);
      break;
  }

}
void fadergb() {
  strip.show();
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(jj*10);
    strip.setBrightness(jj * brightness_ / 10);
    for (int j = 0; j < count_led; j++) {
      strip.setPixelColor(j, strip.Color(GlobalRed_, GlobalGreen_, GlobalBlue_));
    }
    strip.show();
    delay(100/speed_);//recheck
  }
  delay(500/speed_);
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(100-jj*10);
    strip.setBrightness(brightness_ - jj * brightness_ / 10);
    strip.show();
    delay(100/speed_);
  }
  delay(500/speed_);
}

void blinkxmas() {
  strip.clear();
  strip.show();
  all();
    strip.show();
    delay(1000/speed_);
  
    strip.clear();
    strip.show();
  
  delay(1000/speed_);
}

void fadexmas() {
  strip.setBrightness(0);
  strip.show();
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(jj*10);
    strip.setBrightness(jj * brightness_ / 10);
    all();
    strip.show();
    delay(100/speed_);//recheck
  }
  delay(500/speed_);
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(100-jj*10);
    strip.setBrightness(brightness_ - jj * brightness_ / 10);
    strip.show();
    delay(100/speed_);
  }
  delay(500/speed_);
}
void blinkrgb() {
  timedAction.disable();
  strip.clear();
  strip.show();
  for (int jj = 0; jj < count_led; jj++) {
    strip.setPixelColor(jj, strip.Color(GlobalRed_, GlobalGreen_, GlobalBlue_));    
    delay(2);
  }
  strip.show();
  //delay(200);
  delay(1000/speed_);
  strip.clear();
  strip.show();
  delay(1000/speed_);
  timedAction = TimedAction(1000/speed_,blinkrgb);
  timedAction.enable();
}

void blinkl() {
  timedAction.disable();
  strip.setBrightness(brightness_);
  fixedrainbow();
  strip.show();
  delay(1000/speed_);
  strip.clear();
  //strip.setBrightness(0);
  strip.show();
  delay(1000/speed_);
  timedAction = TimedAction(1000/speed_,blinkl);
  timedAction.enable();
}

void faderandom() {
  //timedAction.disable();
  strip.clear();
  strip.show();
  strip.setBrightness(brightness_);
  for (int j = 0; j < count_led; j++) {
        int r_, g_, b_;
                do{
                   r_=random(0,255);
                   g_=random(0,255);
                   b_=random(0,255);
                }while(r_+g_+b_<50);
                strip.setPixelColor(j, r_, g_, b_);//strip.Color(random(0, 255), random(0, 255), random(0, 255)));
  }
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(jj*10);    
    if (jj == 1) {
      

    }
      strip.setBrightness(jj * brightness_ / 10);
      strip.show();
      strip.setBrightness(jj * brightness_ / 10);
    //}
    strip.show();
    delay(100/speed_);//recheck
  }
  delay(500/speed_);
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(100-jj*10);
    strip.setBrightness(brightness_ - jj * brightness_ / 10);
    strip.show();
    delay(100/speed_);//recheck
  }
  delay(500/speed_);
  //timedAction = TimedAction(1000/speed_,faderandom);
  //timedAction.enable();
}

void RUN(){
  String mssg = "RUN";
  timedAction.disable();
  //Serial.println("rec__: ");
  //Serial.println( mssg.length() ); 
  strip.clear();
  strip.show();
  strip.setBrightness(brightness_);
  int line = 0;
  for (int im = 0; im < mssg.length(); im++) {
    //Serial.println( mssg );
    mssg.toUpperCase();
    //Serial.println( mssg );
    //Serial.println( im );
    char mssg_letter = mssg.charAt(im);
    //Serial.println( mssg_letter );
    int mssg_int = (int) mssg_letter - A_int; //+11;//atoi(&mssg_letter)-A_int;
    if (mssg_int >= 8 && mssg_int <= 16) mssg_int = 16 - (mssg_int - 8);
    //if(mssg_int>27)mssg_int=mssg_int+3;
    //Serial.println( mssg_int );

    int mssg_int2 = mssg_int;


    //if(mssg_int >=11 && mssg_int < 40){
    if (mssg_int >= 0 && mssg_int < 27) {
      if (rgbcolors) {
        if ((mssg_int % 3) == 0) strip.setPixelColor(mssg_int2, strip.Color(0, 255, 0));
        else if ((mssg_int % 2) == 0) strip.setPixelColor(mssg_int2, strip.Color(255, 0, 0));
        else strip.setPixelColor(mssg_int2, strip.Color(0, 0, 255));
      }
      else {
        if (mssg_int == 0 || mssg_int == 5 || mssg_int == 12 || mssg_int == 18 || mssg_int == 19 || mssg_int == 23) strip.setPixelColor(mssg_int2, strip.Color(255, 255, 0)); //amarillo
        else if (mssg_int == 1 || mssg_int ==  4 || mssg_int ==  7 || mssg_int == 16 || mssg_int == 20) strip.setPixelColor(mssg_int2, strip.Color(51, 51, 255)); //azul
        else if (mssg_int == 2 || mssg_int == 6 || mssg_int == 15 || mssg_int == 10 || mssg_int == 11 || mssg_int == 8 || mssg_int == 21 || mssg_int == 24 || mssg_int == 25) strip.setPixelColor(mssg_int2, strip.Color(255, 51, 255)); //violeta
        else strip.setPixelColor(mssg_int2, strip.Color(102, 255, 255)); //celeste
        //if(mssg_int== 1 || mssg_int==  4 || mssg_int==  7 || mssg_int==  8 || mssg_int== 20) strip.setPixelColor(mssg_int, strip.Color(255, 102, 255)); //celeste
      }
      strip.show();
      delay(1000/speed_);
    }

    //strip.setPixelColor(mssg_int2, strip.Color(0, 0, 0));
    //strip.show();
    //delay(500/speed_);
  }

  strip.show();
  
  
}


void madness() {  
  timedAction.disable();
  //strip.clear();
  //strip.show();
  strip.setBrightness(brightness_);
  for (int ji = 0; ji < random(1, 4); ji++) {
    for (int j = 0; j < count_led; j++) {
      strip.setPixelColor(random(0,26), strip.Color(random(0, 255), random(0, 255), random(0, 255)));
      strip.show();
      strip.setPixelColor(random(0,26), strip.Color(0, 0, 0));
      strip.show();
  }
  strip.show();
  delay(100);
  }
  timedAction = TimedAction(1000/speed_,madness);
  timedAction.enable();
}


void blinkrandom() {
  //timedAction.disable();
  strip.clear();
  strip.show();  
  for (int j = 0; j < count_led; j++) {
    int r_, g_, b_;
                do{
                   r_=random(0,255);
                   g_=random(0,255);
                   b_=random(0,255);
                }while(r_+g_+b_<50);
                strip.setPixelColor(j, r_, g_, b_);//strip.Color(random(0, 255), random(0, 255), random(0, 255)));
  }
  strip.setBrightness(brightness_);
  strip.show();
  strip.setBrightness(brightness_);
  
  delay(1000/speed_);
  strip.clear();
 // timedAction = TimedAction(1000/speed_,blinkrandom);
 // timedAction.enable();
}

void faderainbow() {
  timedAction.disable();
  strip.clear();
  strip.show();
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(jj*10);
    strip.setBrightness(jj * brightness_ / 10);
    if (jj == 1) fixedrainbow();
    strip.show();
    delay(100/speed_);
  }
  delay(500/speed_);
  for (int jj = 1; jj <= 10; jj++) {
    //strip.setBrightness(100-jj*10);
    strip.setBrightness(brightness_ - jj * brightness_ / 10);
    strip.show();
    delay(100/speed_);//recheck
  }
  delay(500/speed_);
  timedAction = TimedAction(1000/speed_,faderainbow);
  timedAction.enable();
}

void blinkrainbow() {
  timedAction.disable();
  strip.clear();
  strip.show();
  strip.setBrightness(brightness_);
  fixedrainbow();
  strip.show();
  timedAction = TimedAction(1000/speed_,blinkl);
  timedAction.enable();
}


void blinkpurergb() {
  //timedAction.disable();
  strip.clear();
  strip.show();
  strip.setBrightness(brightness_);
  all();
  delay(1000/speed_);
  strip.clear();
  strip.show();
  //delay(1000);
  //timedAction = TimedAction(1000/speed_,blinkpurergb);
  //timedAction.enable();
}


void all() {
  for (int mssg_int = 0; mssg_int < 26; mssg_int++) {
    int i_ = 25 - mssg_int;
    if (rgbcolors) {
      if ((mssg_int % 3) == 0) strip.setPixelColor(i_, strip.Color(255, 0, 0));
      else if ((mssg_int % 2) == 0) strip.setPixelColor(i_, strip.Color(0, 255, 0));
      else strip.setPixelColor(i_, strip.Color(0, 0, 255));
    }
    else {
      if (mssg_int == 0 || mssg_int == 5 || mssg_int == 12 || mssg_int == 18 || mssg_int == 19 || mssg_int == 23) strip.setPixelColor(i_, strip.Color(255, 255, 0)); //amarillo
      else if (mssg_int == 1 || mssg_int ==  4 || mssg_int ==  7 || mssg_int == 16 || mssg_int == 20) strip.setPixelColor(i_, strip.Color(51, 51, 255)); //azul
      else if (mssg_int == 2 || mssg_int == 6 || mssg_int == 15 || mssg_int == 10 || mssg_int == 11 || mssg_int == 8 || mssg_int == 21 || mssg_int == 24 || mssg_int == 25) strip.setPixelColor(i_, strip.Color(255, 51, 255)); //violeta
      else strip.setPixelColor(i_, strip.Color(102, 255, 255)); //celeste
      //if(mssg_int== 1 || mssg_int==  4 || mssg_int==  7 || mssg_int==  8 || mssg_int== 20) strip.setPixelColor(mssg_int, strip.Color(255, 102, 255)); //celeste
    }
  }
  strip.show();

}

void fixedrainbow() {
  timedAction.disable();
  //rainbowCycle(2);
  //timedAction = TimedAction(2560, rainbow);
  //timedAction.enable();
  for (int j = 0; j < 256 * 1; j++) { // 5 cycles of all colors on wheel

    for (int i = 0; i < strip.numPixels(); i++) {

      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    //strip.show();
    delay(2);

  }
  strip.show();
}


void rainbow() {
  timedAction.disable();
  rainbowCycle(2);
  timedAction = TimedAction(2560, rainbow);
  timedAction.enable();
}


uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}



void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  //strip.setBrightness(100);
  strip.setBrightness(brightness_);
  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    if (msg_all == "rainbow") {
      for (i = 0; i < strip.numPixels(); i++) {

        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(wait);
    }
  }
}
