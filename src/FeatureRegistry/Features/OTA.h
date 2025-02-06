#pragma once

#include <ArduinoOTA.h>
#include "./Logging.h"
#include "../Feature.h"
#include "../../services/WebServer.h"

ArRequestHandlerFunction getUpdateForm = ([](AsyncWebServerRequest *request)
                                          { request->send(200, "text/html", F("<form method='POST' action='/update' accept='application/octet-stream' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>")); });

ArRequestHandlerFunction onPostUpdate = ([](AsyncWebServerRequest *request)
                                         {
    boolean shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response); });

ArUploadHandlerFunction onUploadUpdate = ([](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
                                          {
    if (!index)
    {
        Serial.printf("Starting Update...\n");
        
        if (!Update.begin(request->contentLength(), U_FLASH)) // was U_FLASH
        {
            LoggerInstance->Error(String("Update failed with error" + Update.getError()));
            Update.printError(Serial);
        }
    }
    if (!Update.hasError())
    {
        Serial.printf("No error, starting write %d data (index: %d)...\n", len, index);

        if (Update.write(data, len) != len)
        {
            Serial.printf("Write length mismatch, error occurred during write.\n");
            Update.printError(Serial);
        }
    } else {
        Serial.printf("Error occurred during update.\n");
        Update.printError(Serial);
    }
    if (final)
    {
        Serial.printf("Final chunk received.\n");

        if (Update.end(true))
        {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"15\"></head><body>Update done, page will be refreshed.</body></html>");
            response->addHeader("Refresh", "15");
            request->send(response);
            LoggerInstance->Info("Update finished, will restart...");
            delay(300);
            ESP.restart();
        }
        else
        {
            Serial.printf("Error happened, printing to serial.\n");
            Update.printError(Serial);
        }
    } });

Feature *OtaUpgrade = new Feature("OTA", []()
                                  {
    server.on("/update", HTTP_GET, getUpdateForm);
    server.on("/update", HTTP_POST, onPostUpdate, onUploadUpdate);
  return FeatureState::RUNNING; }, []() {});
