#pragma once

#include <sstream>
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
	int writePatchFile(std::stringstream & patchText, FileSettings & fileSettings, const nlohmann::json & sourceJson, const nlohmann::json & intermediaryJson);
};
