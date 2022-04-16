#include "cSML.h"
#include "globals.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define LED_BUILTIN  2

extern Adafruit_SH1106 display;

const char hdrEnergy[8] =    {0x77, 7, 1, 0,  1, 8, 0, 0xFF};
const int offsEnergy = 11;
const char hdrMeterId[8] = {0x77, 7, 1, 0,  0, 0, 9, 0xFF};
const int offsMeterId = 7;
const char hdrPower[8] =     {0x77, 7, 1, 0, 10, 7, 0, 0xFF};
const int offsPower = 7;
bool cSML::readData()
{
  int flashcount = 0;
  long start = millis();
  while (!Serial2.available())
  {
    flashcount++;
    if (flashcount == 400) 
      digitalWrite(LED_BUILTIN, LOW);       // Turn the LED on 
    else if (flashcount > 500) 
    {
      digitalWrite(LED_BUILTIN, HIGH);      // Turn the LED off 
      flashcount=0;
    }
    else 
      delay(5);  // wait 5 ms for new packets
 
    if((millis() - start) > 3000)
      return false;
    if(!digitalRead(PIN_BUTTON))
      return false;
  }
    // We got some bytes. read until next pause
  Serial2.setTimeout(300);   // Set Timeout to 500ms.
  _serindex = Serial2.readBytes(_datagram,1000);
 _tick = millis();
  memcpy(_protocol, _datagram, _serindex);
  memset(&_protocol[_serindex], 0, sizeof(_protocol) - _serindex);
  _countProt++;
  return true;
}

void cSML::showBuffer(int startIdx, bool dispOn)
{
  int i = startIdx;
  display.clearDisplay();
  if(dispOn)
  {
    display.setCursor(0,0);
    for(int j = 0; j < 8; j++)
    {
      display.printf("%03X:%02hhX%02hhX%02hhX%02hhX ",i, _protocol[i], _protocol[i+1], _protocol[i+2], _protocol[i+3]);
      i += 4;
      display.printf("%02hhX%02hhX%02hhX%02hhX\n", _protocol[i], _protocol[i+1], _protocol[i+2], _protocol[i+3]);
      i += 4;
    }
  }
  display.display();
}

void cSML::parseData()
{
  if(checkHeader())
  {
    int offs = searchPattern(hdrEnergy, sizeof(hdrEnergy),_protocol, sizeof(_protocol));
    if(offs >= 0)
    {
      offs += sizeof(hdrEnergy) + offsEnergy;
      int kwh = bufToInt(&_protocol[offs]);
      _kwh = (float)kwh / 10000.0;
      if(_countProt < 2)
        _kwhLast = _kwh;
    }
    offs = searchPattern(hdrMeterId, sizeof(hdrMeterId),_protocol, sizeof(_protocol));
    if(offs >= 0)
    {
      offs += sizeof(hdrMeterId) + offsMeterId;
      
      _meterID[0] = _protocol[offs];
      _meterID[1] = _protocol[offs + 1];
      _meterID[2] = _protocol[offs + 2];
      _meterID[3] = '0';
      _meterID[4] = '0';
      int id = bufToInt(&_protocol[offs + 4]);
      
      snprintf(&_meterID[5], sizeof(_meterID) - 5, "%08i", id); 
    }

    // calculate power
    int dt = _tick - _tickLast;
    float dp = _kwh - _kwhLast;
    if(dt <  0)
    {
      _tickLast = _tick;
      _kwhLast = _kwh;
    }
    else if((dt > 60000)|| (dp > 0.05))
    {
      _integrationTime = (_tick - _tickLast) / 1000;
      _deltaWh = (_kwh - _kwhLast) * 1000;
      _power = _deltaWh * 3600/ _integrationTime;
      _kwhLast = _kwh;
      _tickLast = _tick;
      _countCalc++;
      if(_logIndex < BUF_LEN)
      {
        _logTime[_logIndex] = _tick / 1000;
        _logkwh[_logIndex] = _kwh;
        _logIndex++;
      }
    }
  }
}

int cSML::bufToInt(const char*buf)
{
  int retVal = *buf << 24;
  retVal += *(buf + 1) << 16;
  retVal += *(buf + 2) << 8;
  retVal += *(buf + 3);
  return retVal;
}


bool cSML::checkHeader()
{ 
  bool retVal = (_protocol[0] == 0x1B) && (_protocol[1] == 0x1B) &&
                (_protocol[2] == 0x1B) && (_protocol[3] == 0x1B) &&
                (_protocol[4] == 0x01) && (_protocol[5] == 0x01) &&
                (_protocol[6] == 0x01) && (_protocol[7] == 0x01);
  return retVal;
}

int cSML::searchPattern(const char* pattern, int len, const char* buf, int bufLen)
{
  int maxIdx = bufLen - len + 1;
  int retVal = -1;
  for(int i = 0; i < maxIdx; i++)
  {
    bool found = true;
    for(int j = 0; j < len; j++)
    {
      if(buf[i + j] != pattern[j])
      {
        found = false;
        break;
      }
    }
    if(found)
    {
      retVal = i;
      break;
    }
  }
  return retVal;
}

void cSML::showMeterData(bool dispOn)
{
  display.clearDisplay();
  if(dispOn)
  {
    display.setCursor(0,0);
    display.printf("  ID: %s\n" , _meterID);
    display.printf(" kWh: %.4f\n" , _kwh);
    display.printf("Watt: %.1f\n", _power);
    display.printf(" Rcv: %i\n", _countProt);
    display.printf("Calc: %i\n", _countCalc);
    display.printf("Logi: %i\n", _logIndex);
  }
  display.display();  
}
