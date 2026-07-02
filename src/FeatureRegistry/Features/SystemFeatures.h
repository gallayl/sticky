#include "AsyncJson.h"
#include "../registeredFeatures.h"
#include "../Feature.h"
#include "./wifiCommand.h"
#include "./infoCustomCommand.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../FeatureRegistry.h"
#include "../../services/WebServer.h"
#include "../../utils/Json.h"

CustomCommand *resetCommand = new CustomCommand("restart", [](String command)
                                                {
    ESP.restart();
    return String("{\"event\": \"restart\"}"); });

CustomCommand *getRegisteredFeatures = new CustomCommand("getRegisteredFeatures", [](String command)
                                                         { return jsonToString(registeredFeatures); });

ArRequestHandlerFunction getFeaturesAction = [](AsyncWebServerRequest *request)
{
    request->send(200, MIME_json, jsonToString(registeredFeatures));
};

ArRequestHandlerFunction reset = [](AsyncWebServerRequest *request)
{
    ESP.restart();
    request->send(200, MIME_json, "{\"event\": \"restart\"}");
};

ArRequestHandlerFunction getInfoAction = [](AsyncWebServerRequest *request)
{
    AsyncJsonResponse *resp = new AsyncJsonResponse();
    JsonDocument info = getInfo();
    resp->setCode(200);
    resp->getRoot().set(info);
    resp->setLength();
    request->send(resp);
};

Feature *SystemFeatures = new Feature("SystemFeatures", []()
                                      {
    CommandInterpreterInstance->RegisterCommand(*wifiCommand);
    CommandInterpreterInstance->RegisterCommand(*resetCommand);
    CommandInterpreterInstance->RegisterCommand(*getRegisteredFeatures);
    CommandInterpreterInstance->RegisterCommand(*infoCustomCommand);

    server.on("/features", HTTP_GET, getFeaturesAction);
    server.on("/restart", HTTP_POST, reset);
    server.on("/info", HTTP_GET, getInfoAction);
    return FeatureState::RUNNING; }, []() {

                                      });