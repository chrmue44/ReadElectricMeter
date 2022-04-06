# Read Electric Meter

Software for an ESP32 based device to read out an electric meter and serve a web page with information about electrical energy consumption.

## Introduction

German electric meters (and maybe others too)  provide an infrared interface that continuously sends information about the power consumption.  
The protocol used for this is SML.

## Hardware
The device is based on an ESP32 with an infrared reading unit that is connected to a serial interface. 
The connection to the home network is established via the ESP32 integrated WIFI interface.
An OLED display is connected as well to display WIFI status, power consumption or debug information.

