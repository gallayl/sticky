#pragma once

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <WiFiClientSecure.h>
#include "../config.h"
#include "../FeatureRegistry/Features/Logging.h"

String getSignalStrength(int32_t rssi)
{
    if (rssi > -30)
    {
        return F("Amazing");
    }
    else if (rssi > -67)
    {
        return F("Very good");
    }
    else if (rssi > -70)
    {
        return F("Okay (not good, not terrible)");
    }
    else if (rssi > -80)
    {
        return F("Not good");
    }
    else if (rssi > -90)
    {
        return F("Unusable");
    }
    return F("Unknown");
}

String getEncryptionType(uint8_t type)
{
    switch (type)
    {
    case ENC_TYPE_WEP:
        return F("ENC_TYPE_WEP");
    case ENC_TYPE_TKIP:
        return F("ENC_TYPE_TKIP");
    case ENC_TYPE_CCMP:
        return F("ENC_TYPE_CCMP");
    case ENC_TYPE_NONE:
        return F("ENC_TYPE_NONE");
    case ENC_TYPE_AUTO:
        return F("ENC_TYPE_AUTO");
    }
    return "Unknown";
}

void startStaMode(String ssid, String staPassPharse)
{
    if (WiFi.getMode() == WIFI_AP && WiFi.begin() != WL_CONNECTED)
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ssid, staPassPharse);
    }
}

void initWifi()
{

#ifdef ESP32
    WiFi.onEvent(WiFiEventHandlerESP32);
#endif

#ifdef ESP8266

    WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &event)
                                { LoggerInstance->Info(F("Connected to access point")); });

    WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
                                   { LoggerInstance->Info(F("Disconnected from WiFi access point")); });

    WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event)
                            { LoggerInstance->Info("Obtained IP address: " + WiFi.localIP().toString()); });

    WiFi.onStationModeAuthModeChanged([](const WiFiEventStationModeAuthModeChanged &event)
                                      { LoggerInstance->Info(F("Authentication mode of access point has changed")); });

    WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected &event)
                                      { LoggerInstance->Info(F("Client connected")); });

    WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected &event)
                                         { LoggerInstance->Info(F("Client disconnected")); });

    WiFi.onSoftAPModeProbeRequestReceived([](const WiFiEventSoftAPModeProbeRequestReceived &event)
                                          { LoggerInstance->Info(F("Received probe request")); });

    WiFi.onWiFiModeChange([](const WiFiEventModeChange &event)
                          { LoggerInstance->Info(F("WiFi mode changed")); });
#endif

    WiFi.mode(WIFI_AP);
    WiFi.begin();
    wl_status_t state = (wl_status_t)WiFi.waitForConnectResult();

    if (state != WL_CONNECTED)
    {
        LoggerInstance->Error(F("Failed to connect to access point"));
        startStaMode(STA_SSID, STA_PASSPHARSE);
    }
    else
    {
        LoggerInstance->Info(F("Connected to access point"));
    }
}