#ifndef C_SML
#define C_SML

#include <Arduino.h>

/*
 * receive and decode SML protocol
 */
class cSML
{
  public:
    cSML() {}
    void initComm(int pinRx, int pinTx) { Serial2.begin(9600, SERIAL_8N1, pinRx, pinTx); }

    bool readData();
    void parseData();
    void showBuffer(int startIdx, bool dispOn);
    void showMeterData(bool dispOn);
    const char* getBuf() { return _protocol; }
    float getKwh() { return _kwh; }
    float getPower() { return _power; }
    float getDeltaWh() { return _deltaWh; }
    float getIntegrationTime() { return _integrationTime; }
    int getRcvCount() { return _countProt; }
    int getCalcCount() { return _countCalc; }
  private:
    bool checkHeader();
    int searchPattern(const char* pattern, int len, const char* buf, int bufLen);
    int bufToInt(const char*buf);

    
  private:
    char _datagram[1000];    // array that will hold the received bytes 
    char _protocol[1000];    // array that will hold a complete protocol
    int  _serindex = 0;      // index of serInString[] in which to insert the next incoming byte
    int _countProt = 0;      // count received protocols
    int _countCalc = 0;      // count calculations
    float _kwh;              // kWh read from meter
    char _meterID[16];       // meter ID
    long _tick;              // last time tick received protocol
    long _tickLast;          // last tick to calc power;
    float _kwhLast;          // last kwh at time tick
    float _power;            // calculated power [W]
    float _deltaWh;          // amount of Wh since last calculation
    float _integrationTime;  // integration time interval of last calculation
};

#endif //#ifndef C_SML
