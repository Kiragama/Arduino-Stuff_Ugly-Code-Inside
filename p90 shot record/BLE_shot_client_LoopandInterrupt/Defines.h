#pragma once
#define PIN 18  
#define WAKEPIN 35
#define TRACKERNAME "Shot tracker Display"
#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL
#define SLEEPTIME 120 //seconds
#define DELAYFACTOR 50 // 0.224 was recorded as less then the shot time. Because the sensor stays high for a while, this might be fine tunned to record single shots but if signal high for longer then threshold, it triggers interrupt again. It already does most of this, just need to tune this

#define USING_INTERRUPT false
#define TRANSMIT false
#define SLEEPENABLED false

unsigned volatile static long last_time;
unsigned volatile static long last_transmit;
RTC_DATA_ATTR volatile uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used
static bool notFound = false;
int32_t scancount = 0;
bool serverconnecting = false;
volatile int lockcounter = 0;

