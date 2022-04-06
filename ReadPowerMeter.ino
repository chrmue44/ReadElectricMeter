
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include "cWifi.h"
#include "wifiCredentials.h"
#include "cSML.h"
#include "globals.h"
#include "cWebServer.h"

//******************** globals  ************************

#define TIMEOUT_DISPLAY 60000  // time to turn off display after last button press
enum enMode
{
  WIFI_CONNECT,
  DISP_PWR,
  DISP_WIFI,
  DISP_BUF,
};

Adafruit_SH1106 display(PIN_OLED_SDA, PIN_OLED_SCL);
cWifi wifi;
cSML sml;

int bufIdx = 0;                      // start index for display of buffer
enMode mode = enMode::WIFI_CONNECT;  // display mode
bool buttonOld;                      // old button state
int buttonTime = 0;                  // time when button was pressed
bool displayOn = true;

//******************** initialization  ************************

void initDisplay()
{
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("*************\n");
  display.println("*  Startup  *\n");
  display.println("*************\n");
  display.display();
}


void setup() 
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  sml.initComm(PIN_RX2, PIN_TX2);
  delay(100);
  initDisplay();  
  blink(3,500,500,100);   // Signal startup
  delay(500);
  display.clearDisplay();
  mode = enMode::DISP_WIFI;
  wifi.init(ssid, passwd);
}


//
//  Blink function 
//
void blink(int count, int durationon, int durationoff, int delayafter) {
  for (int i=0; i < count; i++) {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on 
    delay(durationon);
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    delay(durationoff);
  }
  delay(delayafter);
}



//******************** main ************************


void handleModeSwitch()
{
  if(!wifi.isConnected())
    mode = enMode::WIFI_CONNECT;
  else
  {
    if(mode == enMode::WIFI_CONNECT)
    {
      mode = enMode::DISP_PWR;
      cWebServer::inst()->init(&sml);
      buttonTime = millis();
    }
    else if(millis() > (buttonTime + TIMEOUT_DISPLAY))
    {
      displayOn = false;
    }
    bool buttonNew = digitalRead(PIN_BUTTON);
    if(buttonOld && !buttonNew)
    {
      buttonTime = millis();
      if(!displayOn)
        displayOn = true;      
      else
      {
        switch(mode)
        {
          case enMode::DISP_PWR:
            mode = enMode::DISP_WIFI;
            break;
 
          case enMode::DISP_WIFI:
            mode = enMode::DISP_BUF;
            bufIdx = 0;
            break;
        
          case enMode::DISP_BUF:
            if (bufIdx < 0x300)
              bufIdx += 0x40; 
            else
              mode = enMode::DISP_PWR;
            break;          
        }
      }
      delay(70);
    }
    buttonOld = buttonNew;
  }
}


void showReceiveTimeout(bool dispOn)
{
  display.clearDisplay();
  if(dispOn)
  {
    display.setCursor(0,0);
    display.printf("receive timeout");
    display.display();  
  }
}



void loop() 
{
  handleModeSwitch();
  switch(mode)
  {
    case enMode::WIFI_CONNECT:
      wifi.showStatus(displayOn);
      break;
      
    case enMode::DISP_PWR:
      {
        bool ret = sml.readData();
        if(ret)
        {
          sml.parseData();
          sml.showMeterData(displayOn);
        }
        else
        {
          showReceiveTimeout(displayOn);
        }
      }
      break;
    
    case enMode::DISP_WIFI:
      wifi.showStatus(displayOn);
      break;
      
    case enMode::DISP_BUF:
      sml.showBuffer(bufIdx, displayOn);
      break;
  }
  cWebServer::inst()->serve();
}
