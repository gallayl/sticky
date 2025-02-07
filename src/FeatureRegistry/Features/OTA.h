#pragma once

#include "./Logging.h"
#include "../Feature.h"
#include "../../services/WebServer.h"

#define REFRESH_TIMEOUT_AFTER_UPDATE "30"

ArRequestHandlerFunction getUpdateForm = ([](AsyncWebServerRequest *request)
                                          { request->send(200, MIME_html, F("<form method='POST' action='/update' accept='application/octet-stream' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>")); });

ArRequestHandlerFunction getRedirectPage = ([](AsyncWebServerRequest *request)
                                            {
                                                             AsyncWebServerResponse *response = request->beginResponse(200, MIME_html, "<html><head><meta http-equiv=\"refresh\" content=\"30\"></head><body>Update done, page will be refreshed.</body></html>");
    response->addHeader("Refresh", REFRESH_TIMEOUT_AFTER_UPDATE);
    request->send(response); });

ArRequestHandlerFunction onPostUpdate = ([](AsyncWebServerRequest *request)
                                         {
    boolean shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, MIME_plainText, shouldReboot ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response); });

ArUploadHandlerFunction onUploadUpdate = ([](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
                                          {

                                            Update.runAsync(true);

        if (Update.hasError()){
            Update.printError(Serial);
            request->send(500, MIME_plainText, "Update error");
            return;
        }
    if (!index)
    {
        if (!Update.begin(request->contentLength(), U_FLASH))
        {
            Update.printError(Serial);
        }
    }
    if (!Update.hasError())
    {
        if (Update.write(data, len) != len)
        {
            Update.printError(Serial);
        }
    } else {
        Update.printError(Serial);
    }
    if (final)
    {
        if (Update.end(true))
        {
            getRedirectPage(request);
            delay(1000);
            LoggerInstance->Info("Update done, rebooting...");
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    } });

Feature *OtaUpgrade = new Feature("OTA", []()
                                  {
    server.on("/update", HTTP_GET, getUpdateForm);
    server.on("/update", HTTP_POST, onPostUpdate, onUploadUpdate);
  return FeatureState::RUNNING; }, []() {});
