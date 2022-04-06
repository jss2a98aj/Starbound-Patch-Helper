#include "global_settings.h"

#include <fstream>
#include "utilities.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

MasterSettings::MasterSettings(fs::path settingsPath) {
    if (!fs::exists(settingsPath) || loadSettings(fetchJson(settingsPath))) {
        std::stringstream settingsText;
        writeSettings(settingsText);
        writeStringStreamToPath(settingsText, settingsPath);
    }
    postLoad();
}

bool MasterSettings::loadSettings(json settingsJson) {
    //TODO: Handle invalid types more gracefully.
    bool missingSettings = false;
    if (settingsJson.contains("baselinePatchStyleName")) {
        baselinePatchStyleName = settingsJson["baselinePatchStyleName"];
    } else {
        missingSettings = true;
    }
    if (settingsJson.contains("overwriteFiles")) {
        overwriteFiles = settingsJson["overwriteFiles"];
    } else {
        missingSettings = true;
    }
	if (settingsJson.contains("useInverseTestOps")) {
        useInverseTestOps = settingsJson["useInverseTestOps"];
    } else {
        missingSettings = true;
    }
    if (settingsJson.contains("useOperationSets")) {
        useOperationSets = settingsJson["useOperationSets"];
    } else {
        missingSettings = true;
    }
    return missingSettings;
}

void MasterSettings::writeSettings(std::stringstream & settingsText) {
    settingsText << "//User added comments in this file are not preserved when it is updated.\n"
        << '{'
        << "\n  //Loaded from \\patch_styles\\*.json"
        << "\n  \"baselinePatchStyleName\" : \"" << baselinePatchStyleName << "\","
        << "\n  //Skip prompts to delete files. Not suggested for normal use."
        << "\n  \"overwriteFiles\" : " << (overwriteFiles ? "true" : "false") << ','
        << "\n  //Inverse tests are used by Starbound to see if a value is present or not, regardles of its contents."
        << "\n  \"useInverseTestOps\" : " << (useInverseTestOps ? "true" : "false") << ','
        << "\n  //Operation sets are used by Starbound to allow more than one set of operations to be applied from a patch file."
        << "\n  \"useOperationSets\" : " << (useOperationSets ? "true" : "false")
        << "\n}\n";
}

void MasterSettings::postLoad() {
    const fs::path patchStylePath = fs::current_path() /= ("config/patch_styles/" + baselinePatchStyleName + ".json");
    baselinePatchStyle = PatchStyleSettings(patchStylePath);
}

//Getters

std::string MasterSettings::getBaselinePatchStyleName() { return baselinePatchStyleName; }
const bool MasterSettings::getOverwriteFiles() { return overwriteFiles; }
const bool MasterSettings::getUseInverseTestOps() { return useInverseTestOps; }
const bool MasterSettings::getUseOperationSets() { return useOperationSets; }
