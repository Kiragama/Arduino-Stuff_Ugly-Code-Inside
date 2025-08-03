#pragma once
#define PIN 0
#define TRACKERNAME "Shot tracker Display"
#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL



unsigned volatile static long last_time;
RTC_DATA_ATTR volatile static uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used
static bool notFound = false;
int32_t scancount = 0;
bool serverconnecting = false;
