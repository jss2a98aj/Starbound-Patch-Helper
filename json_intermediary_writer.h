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
	int writeIntermediaryFile(std::stringstream & intermediaryText, FileSettings & fileSettings, const nlohmann::json & sourceJson);
};
