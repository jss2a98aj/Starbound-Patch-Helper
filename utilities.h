#pragma once

#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>

/**
 * Strip JSON comments from strings.
 * 
 * @param text The text to remove JSON comments from.
 */
void stripJsonComments(std::string & text);

/**
 * Converts newlines in JSON values to breakout newlines.
 * 
 * @param text The text to convert newlines in.
 */
void convertJsonValueNewlinesToBreakout(std::string & text);

/**
 * Converts breakout newlines in JSON values to newlines.
 * 
 * @param text The text to convert breakout newlines in.
 */
void convertNewlineBreakoutsToNewline(std::string & text);


/**
 * Converts quotes in JSON values to breakout quotes.
 * 
 * @param text The text to convert quotes in.
 */
void convertQuoteToBreakoutQuote(std::string & text);

/**
 * Converts breakout newlines in values to newlines".
 * 
 * @param text The text to have a character replaced in.
 * @param x The character to replace.
 * @param replacement The string to replace the character with.
 * @return The position of the character replaced, -1 if none.
 */
int replaceFirstOfX(std::string & text, const char x, const std::string replacement);

/**
 * Retrieves a Text file from the path.
 * 
 * @param filePath The path the file is at.
 * @return The file document as std::string.
 */
const std::string fetchText(std::filesystem::path filePath);

/**
 * Retrieves a JSON file from the path.
 * 
 * @param filePath The path the file is at.
 * @param valuesHaveNewlines If values have actual newlines in them.
 * @return The JSON file as nlohmann::json.
 */
const nlohmann::json fetchJson(std::filesystem::path filePath, bool valuesHaveNewlines);

/**
 * Retrieves a JSON file from the path.
 * 
 * @param filePath The path the file is at.
 * @return The JSON file as nlohmann::json.
 */
const nlohmann::json fetchJson(std::filesystem::path filePath);

/**
 * Writes a string steam to a specific path..
 * 
 * @param stream The string stream to write.
 * @param filePath The path the file should be written to.
 * @return If the file was written.
 */
const bool writeStringStreamToPath(std::stringstream & stream, std::filesystem::path filePath);
