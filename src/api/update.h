#pragma once

#ifdef ESP32
#include <Update.h>
#else
#include <ArduinoOTA.h>
#endif
#include "../FeatureRegistry/Features/Logging.h"

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
        
        if (!Update.begin(request->contentLength(), U_FLASH))
        {
            LoggerInstance->Error(String("Update failed with error" + Update.getError()));
            Update.printError(Serial);
        }
    }
    if (!Update.hasError())
    {
        if (Update.write(data, len) != len)
        {
            Update.printError(Serial);
        }
    }
    if (final)
    {
        if (Update.end(true))
        {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"5\"></head><body>Update done, page will be refreshed.</body></html>");
            response->addHeader("Refresh", "5");
            request->send(response);
            LoggerInstance->Info("Update finished, will restart...");
            delay(300);
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    } });
