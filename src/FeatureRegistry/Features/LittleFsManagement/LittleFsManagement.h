#pragma once
#include "../../Feature.h"
#include "../../../CommandInterpreter/CommandInterpreter.h"
#include "../Logging.h"
#include "./showFileListCustomCommand.h"
#include "./formatCustomCommand.h"

Feature *LittleFsFeature = new Feature("LittleFsFeatures", []() {

#ifdef ESP32
    if (!LittleFS.begin(true))
#else
    if (!LittleFS.begin())
#endif
    {
        LoggerInstance->Error(F("LittleFS not available"));
        return FeatureState::ERROR;
    }

    CommandInterpreterInstance->RegisterCommand(*showFileListCustomCommand);
    CommandInterpreterInstance->RegisterCommand(*formatCustomCommand);
    return FeatureState::RUNNING;
}, []() {
    
});