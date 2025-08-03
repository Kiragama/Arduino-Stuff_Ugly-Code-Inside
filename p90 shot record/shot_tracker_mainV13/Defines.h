#pragma once


#define EEPROM_SIZE 100  //bytes
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "6bc38e09-5e61-4838-8d1b-f3e093a2c5e6"  //server advertises and as long as client has the same UUID and Characteristic UUID. Plenty of security holes here
#define CHARACTERISTIC_UUID "b3286334-8358-4b98-9111-2b3cef9758c9"

#define EEADDRESS 0
#define NAMEADDRESS 4 //floats reserve 4 bytes in size

#define USING_C3 false

#if (USING_C3) 
#define TFT_DC 1
#define TFT_CS 7
#define TFT_RST 2
#define IRPIN 3
#define TEXTSIZE 3

#else
#define TFT_DC 32
#define TFT_CS 15
#define TFT_RST -1
#define IRPIN 14
#define TEXTSIZE 1
#define TEXT_X 5
#define TEXT_Y 20
#define NUM_X 26
#define NUM_Y 70
#define NUMSIZE 2
#endif


#define BACKCOLOUR COLOR_RGB565_BLACK
#define TEXTCOLOUR COLOR_RGB565_OLIVE

#define RELOADPIN 13
#define RELOADRF 27 
#define PAIRBTN 26

