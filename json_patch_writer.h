#pragma once

#include <sstream>
#include "nlohmann/json.hpp"
#include "global_settings.h"
#include "parse_settings.h"

enum patchOperation {
    addValue,
    replaceValue,
    removeValue,
    copyValue,
    moveValue,
    testValue,
    testInverse
};

class JsonPatchWriter {
private:
    PatchStyleSettings baselinePatchStyle;
    bool useInverseTestOps;
    bool useOperationSets;
    int indentModifier;
    int currentOps;
    int currentOpSets;
    
    bool writeOperationSet(std::stringstream & patchText, const PointerSettings & pointerSettings, const nlohmann::json & sourceJson, const nlohmann::json & intermediaryJson);
    bool writeRecursiveOperationSet(std::stringstream & patchText, const PointerSettings & pointerSettings, const nlohmann::json & sourceJson, const nlohmann::json & intermediaryJson);
    void writeOperation(std::stringstream & patchText, std::string path, std::string value, patchOperation operation);
    void writeOperationSetOpener(std::stringstream & patchText);
    void writeOperationSetCloser(std::stringstream & patchText);
    void writeIndent(std::stringstream & patchText);
    void writeColon(std::stringstream & patchText);
public:
    JsonPatchWriter(MasterSettings masterSettings);
    /**
     * Writes a patch file. Some configurations will not comply with official JSON patch standards.
     * 
     * @param patchText The string stream the patches will be written to.
     * @param fileSettings The file extension specific settings to use when making patches.
     * @param sourceJson The JSON patches will assume is used as a base.
     * @param intermediaryJson The JSON patches will try to make the base mimic when applied.
     * @return How many values the resulting patch will add or replace.
     */
    int writePatchFile(std::stringstream & patchText, FileSettings & fileSettings, const nlohmann::json & sourceJson, const nlohmann::json & intermediaryJson);
};
