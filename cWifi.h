#ifndef C_WIFI
#define C_WIFI
#include <WiFi.h>

class cWifi
{
  public:
    cWifi();
    void init(const char* ssid, const char* passwd);
    bool isConnected() { return WiFi.status() == WL_CONNECTED; }
    IPAddress getIpAddress() { return WiFi.localIP(); }
    void showStatus(bool dispOn);
      
  private:
     const char* _pSsid;
};

#endif //#ifndef C_WIFI
