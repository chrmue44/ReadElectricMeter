#ifndef C_WEBSERVER
#define C_WEBSERVER

#include <WebServer.h>
#include <ESPmDNS.h>
#include "cSML.h"

class cWebServer
{
  public:
    void init(cSML* sml);
    void serve();
    static cWebServer* inst();

  protected:
    cWebServer(int port);
    
  private:
    static void handleRoot();
    static void handleRawData();
    static void handleDecoded();
    static void handleCsv();
    static void handleLog(); 
    static void handleNotFound();
    
    
  private:
    static cWebServer* _this;
    bool _started = false;
    WebServer _server;
     cSML* _sml;

};

#endif //#ifndef C_WEBSERVER
