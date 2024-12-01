#pragma once

#include <filesystem>
#include <vector>

enum placeholderCondition {
    never,
    whenPossible,
    fromExists
};

enum placeholderOperation {
    none,
    copy,
    move
};

struct PointerSettings {
    //Shared.
    std::string path = "";
    std::string from = "";
    std::string numericIteratorMarker = "";
    bool reference = false;
    bool convertBreakoutNewlines = false;
    //Intermediary.
    placeholderCondition intermediaryPlaceholderCondition = never;
    std::string intermediaryLabel = "";
    //Patch.
    bool patchTestOperation = true;
    placeholderOperation patchOperationIfPlaceholder = none;
    std::string patchRemoveIfEquals = "";
};

class FileSettings {
private:
    std::string fileExtension;
    bool addPatchMakersComment = false;
    bool valuesContainNewlines = false;
    std::vector<PointerSettings> allPointerSettings;
public:
    FileSettings(std::filesystem::path settingsPath);
    void writeExampleSettings(std::stringstream & settingsText);
    const bool hasPointerSettings();
    //Getters
    const std::string getFileExtension();
    const bool getAddPatchMakersComment();
    const bool getValuesContainNewlines();
    std::vector<PointerSettings> getAllPointerSettings();
};
