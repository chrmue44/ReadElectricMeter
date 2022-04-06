#include "cWifi.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

extern Adafruit_SH1106 display;

cWifi::cWifi()
{
    
}

void cWifi::init(const char* ssid, const char* passwd)
{
  _pSsid = ssid;
  WiFi.begin(ssid, passwd);
}




void cWifi::showStatus(bool dispOn)
{
  display.clearDisplay();  
  if(dispOn)
  {
    display.setCursor(0,0);
    display.println("-- WIFI --");
    if(isConnected())
    {  
      display.println("SSID:");
      display.println(_pSsid);
      display.println("Status:");
      display.println("connected");
      display.println("IP Address:");
      display.println(getIpAddress()); 
    }
    else
    {
      display.println("connecting to:");
      display.println(_pSsid);
      display.println("\nStatus:");
      display.println("disconnected");
    }
  }
  display.display(); 
}
