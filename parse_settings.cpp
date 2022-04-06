#include "parse_settings.h"

#include <iostream>
#include "utilities.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

FileSettings::FileSettings(fs::path settingsPath) {
    fileExtension = '.' + settingsPath.stem().string();
    json settingsJson = fetchJson(settingsPath);

    if (settingsJson.contains("addPatchMakersComment")) {
        addPatchMakersComment = settingsJson["addPatchMakersComment"];
    }
    if (settingsJson.contains("valuesContainNewlines")) {
        valuesContainNewlines = settingsJson["valuesContainNewlines"];
    }

    if (settingsJson.contains("values")) {
        //TODO: Handle invalid types more gracefully.
        json valuesArrayJson = settingsJson["values"];
        for (json::iterator iteration = valuesArrayJson.begin(); iteration != valuesArrayJson.end(); ++iteration) {
            json valuesJson = iteration.value();

            PointerSettings pointerSettings;

            if (valuesJson.contains("path")) {
                pointerSettings.path = valuesJson["path"];
                if (valuesJson.contains("from")) {
                    pointerSettings.from = valuesJson["from"];
                }
                if (valuesJson.contains("numericIteratorMarker")) {
                    pointerSettings.numericIteratorMarker = valuesJson["numericIteratorMarker"];
                }
                if (valuesJson.contains("reference")) {
                    pointerSettings.reference = valuesJson["reference"];
                }
                if (valuesJson.contains("convertBreakoutNewlines")) {
                    pointerSettings.convertBreakoutNewlines = valuesJson["convertBreakoutNewlines"];
                }
                if (valuesJson.contains("intermediaryPlaceholderCondition")) {
                    std::string value = valuesJson["intermediaryPlaceholderCondition"];
                    if (value == "never") {
                        pointerSettings.intermediaryPlaceholderCondition = never;
                    } else if (value == "when possible") {
                        pointerSettings.intermediaryPlaceholderCondition = whenPossible;
                    } else if (value == "from exists") {
                        pointerSettings.intermediaryPlaceholderCondition = fromExists;
                    } else {
                        std::cout << "parse_settings \"" << value << "\" is not a valid value for intermediaryPlaceholderCondition.\n";
                    }
                }
                if (valuesJson.contains("intermediaryLabel")) {
                    pointerSettings.intermediaryLabel = valuesJson["intermediaryLabel"];
                }
                if (valuesJson.contains("patchTestOperation")) {
                    pointerSettings.patchTestOperation = valuesJson["patchTestOperation"];
                }
                if (valuesJson.contains("patchOperationIfPlaceholder")) {
                    std::string value = valuesJson["patchOperationIfPlaceholder"];
                    if (value == "none") {
                        pointerSettings.patchOperationIfPlaceholder = none;
                    } else if (value == "copy") {
                        pointerSettings.patchOperationIfPlaceholder = copy;
                    } else if (value == "move") {
                        pointerSettings.patchOperationIfPlaceholder = move;
                    } else {
                        std::cout << "parse_settings \"" << value << "\" is not a valid value for patchOperationIfPlaceholder.\n";
                    }
                }
                if (valuesJson.contains("patchRemoveIfEquals")) {
                    pointerSettings.patchRemoveIfEquals = valuesJson["patchRemoveIfEquals"].dump();
                    
                }
                allPointerSettings.push_back(pointerSettings);
            }
        }
    }
}

void FileSettings::writeExampleSettings(std::stringstream & settingsText) {
    //TODO: Write example settings.
}

const bool FileSettings::hasPointerSettings() {
    return allPointerSettings.size() >= 1;
}

//Getters

const std::string FileSettings::getFileExtension() { return fileExtension; }
const bool FileSettings::getAddPatchMakersComment() { return addPatchMakersComment; }
const bool FileSettings::getValuesContainNewlines() { return valuesContainNewlines; }
std::vector<PointerSettings> FileSettings::getAllPointerSettings() { return allPointerSettings; }
