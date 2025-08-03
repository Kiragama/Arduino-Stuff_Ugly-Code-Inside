#pragma once

#define EEPROM_SIZE 10  //bytes
#define TRACKERNAME "Shot tracker Display" //Determins which server it connects to. Only ever change this. DO NOT CHANGE SERVICE/CHARACTERISTICS UUIDS.
#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL
#define SLEEPTIME 60 //seconds
#define DELAYFACTOR 50 // 0.224 was recorded as less then the shot time. Because the sensor stays high for a while, this might be fine tunned to record single shots but if signal high for longer then threshold, it triggers interrupt again. It already does most of this, just need to tune this
#define RANDADDRESS 0
#define NAMEADDRESS 4
#define PAIRTIMER 1
#define DEFAULTP 1.23F

#define BATTERY_CHECK_INTERVAL 40000 //only used if using OMG

#define USING_OMGS3 true
#define USING_C3 false
#define USING_INTERRUPT true
#define TRANSMIT true
#define SLEEPENABLED false
#define PRINTING false
#define USING_PAIRMODE true //erase flash if turing off
#define SERIALDELAY 500 //used to ensure that the serial buffer does not get full and cause missing data from serialprints


#if USING_C3
#define PIN 0  
#define WAKEPIN 0
#endif

#if USING_OMGS3
#define PIN 13  
#define WAKEPIN 13
#define PAIRBTN 10
#define RAWOUT 8

#endif

unsigned volatile static long last_time;
unsigned volatile static long last_transmit;
RTC_DATA_ATTR volatile uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used
static bool notFound = false;
int32_t scancount = 0;
bool serverconnecting = false;
volatile int lockcounter = 0;

unsigned long lastBatteryCheck = 0; //only used if using OMG
int pairValue;
bool isInPairMode = false;
bool restartedInPairMode = false;
int pairTime = 0;
String displayName;


