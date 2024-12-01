#pragma once

#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>

void stripJsonComments(std::string & text);

void convertJsonValueNewlinesToBreakout(std::string & text);

void convertNewlineBreakoutsToNewline(std::string & text);

void convertQuoteToBreakoutQuote(std::string & text);

int replaceFirstOfX(std::string & text, const char x, const std::string replacement);

const std::string fetchText(std::filesystem::path filePath);

const nlohmann::json fetchJson(std::filesystem::path filePath, bool valuesHaveNewlines);

const nlohmann::json fetchJson(std::filesystem::path filePath);

const bool writeStringStreamToPath(std::stringstream & stream, std::filesystem::path filePath);
