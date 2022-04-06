#include "cWebServer.h"
cWebServer* cWebServer::_this = nullptr;


cWebServer::cWebServer(int port)
: _server(port)
{

}

cWebServer* cWebServer::inst()
{
  if(_this == nullptr)
    _this = new cWebServer(80);
  return _this;
}

void cWebServer::init(cSML* sml)
{
  if(!_started)
  {
    _sml = sml;
    _server.on("/", handleRoot);
    _server.on("/raw", handleRawData);
    _server.on("/pretty", handleDecoded);
    _server.on("/csv", handleCsv);
    _server.onNotFound(handleNotFound);
    _server.begin();
    _started = true;
  }
}

void cWebServer::handleRawData() 
{
  char temp[2048];
   int idx = 0;
   
 // the content of the HTTP response follows the header:
//  snprintf(temp, sizeof(temp), "SML protocol:<br>");
  int len = 0;
  temp[0] = 0;
  for(int j = 0; j < 0x20; j++)
  {
  //  len = strlen(temp);
  //  snprintf(&temp[len], sizeof(temp) - len,"%04X: ", idx);
    for(int i = 0; i < 8; i++)
    {
      len = strlen(temp);
      snprintf(&temp[len], sizeof(temp) - len, "%02hhX ", _this->_sml->getBuf()[idx]);
      idx++;
    }
    len = strlen(temp);
    snprintf(&temp[len], sizeof(temp) - len,"  ");
    for(int i = 0; i < 8; i++)
    {
      len = strlen(temp);
      snprintf(&temp[len], sizeof(temp) - len, "%02hhX ", _this->_sml->getBuf()[idx]);
      idx++;
    }
    len = strlen(temp);    
    snprintf(&temp[len], sizeof(temp) - len,"<br>");
  }
  _this->_server.send(200, "text/html",temp);
}

#define HEAD_STR   "<head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Power Meter Readout</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>"


void cWebServer::handleDecoded() 
{
  char temp[512];
  snprintf(temp, sizeof(temp),
  "<html>"
  HEAD_STR
  "<body>\
    <h1>Meter Data</h1><p>\
    kwh: %.4f <br>\n\
    Watt: %.1f<br>\n\
    delta Wh: %.1f <br>\n\
    Integration Time[s]: %.1f<br>\n\
    Receive Count: %i<br>\n\
    Calculation Count: %i<br>\n\
    </p></body></html>",\
    _this->_sml->getKwh(), _this->_sml->getPower(),
    _this->_sml->getDeltaWh(),_this->_sml->getIntegrationTime(),
    _this->_sml->getRcvCount(), _this->_sml->getCalcCount());    
    _this->_server.send(200, "text/html",temp);
}

void cWebServer::handleCsv() 
{
  char temp[512];
  snprintf(temp, sizeof(temp),"%.4f,%.1f,%i,%i\n",
    _this->_sml->getKwh(), _this->_sml->getPower(),
    _this->_sml->getRcvCount(), _this->_sml->getCalcCount());    
  _this->_server.send(200, "text/html",temp);
}

void cWebServer::handleRoot() 
{
  char temp[1024];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, sizeof(temp),

  "<html>"
  HEAD_STR
  "<body>\
    <h1>Power Meter Readout</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p><a href=\"/pretty\">Meter Data</a><br> \
       <a href=\"/raw\">Raw Protol Data</a><br>\
       <a href=\"/csv\">Csv Data</a> (comma deselected decoded values)</p>\
  </body>\
</html>",

           hr, min % 60, sec % 60);
  _this->_server.send(200, "text/html", temp);
}


void cWebServer::handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _this->_server.uri();
  message += "\nMethod: ";
  message += (_this->_server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += _this->_server.args();
  message += "\n";

  for (uint8_t i = 0; i < _this->_server.args(); i++) 
  {
    message += " " + _this->_server.argName(i) + ": " + _this->_server.arg(i) + "\n";
  }

  _this->_server.send(404, "text/plain", message);
}


void cWebServer::serve()
{
  _server.handleClient();
  delay(2);//allow the cpu to switch to other tasks  
}
      
