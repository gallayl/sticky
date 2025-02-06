#pragma once
#include "../../Feature.h"
#include "../../../CommandInterpreter/CommandInterpreter.h"
#include "../Logging.h"
#include "./showFileListCustomCommand.h"
#include "./formatCustomCommand.h"

Feature *LittleFsFeature = new Feature("LittleFsFeatures", []()
                                       {

    if (!LittleFS.begin())
    {
        LoggerInstance->Error(F("LittleFS not available"));
        return FeatureState::ERROR;
    }

    CommandInterpreterInstance->RegisterCommand(*showFileListCustomCommand);
    CommandInterpreterInstance->RegisterCommand(*formatCustomCommand);
    return FeatureState::RUNNING; }, []() {

                                       });