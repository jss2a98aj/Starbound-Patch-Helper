#pragma once

#include <sstream>
#include <nlohmann/json.hpp>
#include "parse_settings.h"

class JsonIntermediaryWriter {
private:
    int totalIntermediaryValues = 0;
    bool writePointerValuePair(std::stringstream & intermediaryText, const PointerSettings & pointerSettings, const nlohmann::json & sourceJson);
    bool writeRecursivePointerValuePair(std::stringstream & intermediaryText, const PointerSettings & pointerSettings, const nlohmann::json & sourceJson);
    void writePointerValueStart(std::stringstream & intermediaryText, const PointerSettings & pointerSettings);
public:
    JsonIntermediaryWriter();
    /**
     * Writes an intermediary file. Some configurations will not comply with official JSON standards.
     * 
     * @param patchText The string stream the intermediary JSON will be written to.
     * @param fileSettings The file extension specific settings to use when parsing.
     * @param sourceJson The JSON things are parsed from.
     * @return How many values the resulting intermediary JSON contains.
     */
    int writeIntermediaryFile(std::stringstream & intermediaryText, FileSettings & fileSettings, const nlohmann::json & sourceJson);
};
