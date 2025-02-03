#pragma once
#include <WiFiUdp.h>
#include <stdio.h>
#include <time.h> 
#include "../Feature.h"

#define MY_NTP_SERVER "pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"



time_t getEpochTime()
{
  time_t now;  
  time(&now);
  return now;
}

String getUtcTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,80,"%FT%TZ",timeinfo);
  return String(buffer);
}


Feature *TimeFeature = new Feature("Time", []() {
    configTime(MY_TZ, MY_NTP_SERVER); 
    return FeatureState::RUNNING;
}, [](){});
