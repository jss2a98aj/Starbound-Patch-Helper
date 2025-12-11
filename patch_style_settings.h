#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

class PatchStyleSettings {
	int indentationSpacesPerModifier = 2;
	//Outer wrapper brackets
	bool newLineAfterOuterOpenerBracket = true;
	bool newLineAfterOuterCloserBracket = true;
	bool indentationInOuterBrackets = true;
	//Operation set wrapper brackets (Starbound specific)
	bool newLineAfterOperationSetOpenerBracket = true;
	bool newLineAfterOperationSetCloserBracket = false;
	bool newLineAfterOperationSetCloserBracketComma = true;
	bool indentationInOperationSetBrackets = true;
	//Operation wrapper brackets
	bool newLineAfterOperationOpenerBracket = true;
	bool newLineAfterOperationCloserBracket = false;
	bool newLineAfterOperationCloserBracketComma = true;
	bool indentationInOperationBrackets = true;
	//Operation contents
	bool newLineAfterOperationSegment = true;
	bool spaceBeforeOperationColon = true;
	bool spaceAfterOperationColon = true;

	void loadSettings(nlohmann::json settingsJson);
	void writeSettings(std::stringstream & settingsText);
public:
	PatchStyleSettings();
	PatchStyleSettings(std::filesystem::path settingsPath);
	//Getters
	const int getIndentationSpacesPerModifier();
	//Outer wrapper brackets
	const bool getNewLineAfterOuterOpenerBracket();
	const bool getNewLineAfterOuterCloserBracket();
	const bool getIndentationInOuterBrackets();
	//Operation set wrapper brackets (Starbound specific)
	const bool getNewLineAfterOperationSetOpenerBracket();
	const bool getNewLineAfterOperationSetCloserBracket();
	const bool getNewLineAfterOperationSetCloserBracketComma();
	const bool getIndentationInOperationSetBrackets();
	//Operation wrapper brackets
	const bool getNewLineAfterOperationOpenerBracket();
	const bool getNewLineAfterOperationCloserBracket();
	const bool getNewLineAfterOperationCloserBracketComma();
	const bool getIndentationInOperationBrackets();
	//Operation contents
	const bool getNewLineAfterOperationSegment();
	const bool getSpaceBeforeOperationColon();
	const bool getSpaceAfterOperationColon();
};
