#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include "patch_style_settings.h"

class MasterSettings {
private:
	std::string baselinePatchStyleName = "default";
	bool overwriteFiles = false;
	bool useInverseTestOps = true;
    bool useOperationSets = true;

    bool loadSettings(nlohmann::json settingsJson);
    void writeSettings(std::stringstream & settingsText);
	void postLoad();
public:
	MasterSettings(std::filesystem::path settingsPath);
	PatchStyleSettings baselinePatchStyle;
	//Getters
	std::string getBaselinePatchStyleName();
	const bool getOverwriteFiles();
	const bool getUseInverseTestOps();
	const bool getUseOperationSets();
};
