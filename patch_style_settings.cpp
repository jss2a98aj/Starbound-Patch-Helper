#include "patch_style_settings.h"

#include <fstream>
#include "utilities.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

PatchStyleSettings::PatchStyleSettings() { }

PatchStyleSettings::PatchStyleSettings(std::filesystem::path settingsPath) {
	if (fs::exists(settingsPath)) {
		loadSettings(fetchJson(settingsPath));
	} else {
		std::stringstream settingsText;
		writeSettings(settingsText);
		writeStringStreamToPath(settingsText, settingsPath);
	}
}

void PatchStyleSettings::writeSettings(std::stringstream & settingsText) {
	std::stringstream settingsJson;
	settingsJson << '{'
		<< "\n  \"indentationSpacesPerModifier\" : " << indentationSpacesPerModifier
		<< "\n  //Outer wrapper brackets"
		<< "\n  \"newLineAfterOuterOpenerBracket\" : " << newLineAfterOuterOpenerBracket
		<< "\n  \"newLineAfterOuterCloserBracket\" : " << newLineAfterOuterCloserBracket
		<< "\n  \"indentationInOuterBrackets\" : " << indentationInOuterBrackets
		<< "\n  //Operation set wrapper brackets (Starbound)"
		<< "\n  \"newLineAfterOperationSetOpenerBracket\" : " << newLineAfterOperationSetOpenerBracket
		<< "\n  \"newLineAfterOperationSetCloserBracket\" : " << newLineAfterOperationSetCloserBracket
		<< "\n  \"newLineAfterOperationSetCloserBracketComma\" : " << newLineAfterOperationSetCloserBracketComma
		<< "\n  \"indentationInOperationSetBrackets\" : " << indentationInOperationSetBrackets
		<< "\n  //Operation wrapper brackets"
		<< "\n  \"newLineAfterOperationOpenerBracket\" : " << newLineAfterOperationOpenerBracket
		<< "\n  \"newLineAfterOperationCloserBracket\" : " << newLineAfterOperationCloserBracket
		<< "\n  \"newLineAfterOperationCloserBracketComma\" : " << newLineAfterOperationCloserBracketComma
		<< "\n  \"indentationInOperationBrackets\" : " << indentationInOperationBrackets
		<< "\n  //Operation contents"
		<< "\n  \"newLineAfterOperationSegment\" : " << newLineAfterOperationSegment
		<< "\n  \"spaceBeforeOperationColon\" : " << spaceBeforeOperationColon
		<< "\n  \"spaceAfterOperationColon\" : " << spaceAfterOperationColon
		<< "\n}\n";
}

void PatchStyleSettings::loadSettings(json settingsJson) {
	//TODO: Handle invalid types
	if (settingsJson.contains("indentationSpacesPerModifier")) {
		indentationSpacesPerModifier = settingsJson["indentationSpacesPerModifier"];
	}
	//Outer wrapper brackets
	if (settingsJson.contains("newLineAfterOuterOpenerBracket")) {
		newLineAfterOuterOpenerBracket = settingsJson["newLineAfterOuterOpenerBracket"];
	}
	if (settingsJson.contains("newLineAfterOuterCloserBracket")) {
		newLineAfterOuterCloserBracket = settingsJson["newLineAfterOuterCloserBracket"];
	}
	if (settingsJson.contains("indentationInOuterBrackets")) {
		indentationInOuterBrackets = settingsJson["indentationInOuterBrackets"];
	}
	//Operation set wrapper brackets (Starbound specific)
	if (settingsJson.contains("newLineAfterOperationSetOpenerBracket")) {
		newLineAfterOperationSetOpenerBracket = settingsJson["newLineAfterOperationSetOpenerBracket"];
	}
	if (settingsJson.contains("newLineAfterOperationSetCloserBracket")) {
		newLineAfterOperationSetCloserBracket = settingsJson["newLineAfterOperationSetCloserBracket"];
	}
	if (settingsJson.contains("newLineAfterOperationSetCloserBracketComma")) {
		newLineAfterOperationSetCloserBracketComma = settingsJson["newLineAfterOperationSetCloserBracketComma"];
	}
	if (settingsJson.contains("indentationInOperationSetBrackets")) {
		indentationInOperationSetBrackets = settingsJson["indentationInOperationSetBrackets"];
	}
	//Operation wrapper brackets
	if (settingsJson.contains("newLineAfterOperationOpenerBracket")) {
		newLineAfterOperationOpenerBracket = settingsJson["newLineAfterOperationOpenerBracket"];
	}
	if (settingsJson.contains("newLineAfterOperationCloserBracket")) {
		newLineAfterOperationCloserBracket = settingsJson["newLineAfterOperationCloserBracket"];
	}
	if (settingsJson.contains("newLineAfterOperationCloserBracketComma")) {
		newLineAfterOperationCloserBracketComma = settingsJson["newLineAfterOperationCloserBracketComma"];
	}
	if (settingsJson.contains("indentationInOperationBrackets")) {
		indentationInOperationBrackets = settingsJson["indentationInOperationBrackets"];
	}
	//Operation contents
	if (settingsJson.contains("newLineAfterOperationSegment")) {
		newLineAfterOperationSegment = settingsJson["newLineAfterOperationSegment"];
	}
	if (settingsJson.contains("spaceBeforeOperationColon")) {
		spaceBeforeOperationColon = settingsJson["spaceBeforeOperationColon"];
	}
	if (settingsJson.contains("spaceAfterOperationColon")) {
		spaceAfterOperationColon = settingsJson["spaceAfterOperationColon"];
	}
}

//Getters

const int PatchStyleSettings::getIndentationSpacesPerModifier() { return indentationSpacesPerModifier; }
//Outer wrapper brackets
const bool PatchStyleSettings::getNewLineAfterOuterOpenerBracket() { return newLineAfterOuterOpenerBracket; }
const bool PatchStyleSettings::getNewLineAfterOuterCloserBracket() { return newLineAfterOuterCloserBracket; }
const bool PatchStyleSettings::getIndentationInOuterBrackets() { return indentationInOuterBrackets; }
//Operation set wrapper brackets (Starbound specific)
const bool PatchStyleSettings::getNewLineAfterOperationSetOpenerBracket() { return newLineAfterOperationSetOpenerBracket; }
const bool PatchStyleSettings::getNewLineAfterOperationSetCloserBracket() { return newLineAfterOperationSetCloserBracket; }
const bool PatchStyleSettings::getNewLineAfterOperationSetCloserBracketComma() { return newLineAfterOperationSetCloserBracketComma; }
const bool PatchStyleSettings::getIndentationInOperationSetBrackets() { return indentationInOperationSetBrackets; }
//Operation wrapper brackets
const bool PatchStyleSettings::getNewLineAfterOperationOpenerBracket() { return newLineAfterOperationOpenerBracket; }
const bool PatchStyleSettings::getNewLineAfterOperationCloserBracket() { return newLineAfterOperationCloserBracket; }
const bool PatchStyleSettings::getNewLineAfterOperationCloserBracketComma() { return newLineAfterOperationCloserBracketComma; }
const bool PatchStyleSettings::getIndentationInOperationBrackets() { return indentationInOperationBrackets; }
//Operation contents
const bool PatchStyleSettings::getNewLineAfterOperationSegment() { return newLineAfterOperationSegment; }
const bool PatchStyleSettings::getSpaceBeforeOperationColon() { return spaceBeforeOperationColon; }
const bool PatchStyleSettings::getSpaceAfterOperationColon() { return spaceAfterOperationColon; }
