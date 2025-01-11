#include "utilities.h"

#include <fstream>

using json = nlohmann::json;

namespace fs = std::filesystem;

enum StripMode { structure, quote, commentSingle, commentMulti };

/**
 * Strip JSON comments from strings.
 * 
 * @param text The text to remove JSON comments from.
 */
void stripJsonComments(std::string & text) {
    StripMode mode = structure;
    int commentStart, commentLength;
    char prev = 'U', current = 'U';
    for(int i = 0; i < text.length(); ++i) {
        prev = current;
        current = text[i];
        switch(mode) {
            case structure:
                if(current == '"') {
                    mode = quote;
                } else if(prev == '/') {
                    if(current == '/') {
                        mode = commentSingle;
                        text[i - 1] = ' ';
                        text[i] = ' ';
                    } else if(current == '*') {
                        mode = commentMulti;
                        text[i - 1] = ' ';
                        text[i] = ' ';
                    }
                }
            break;
            case quote:
                if(prev != '\\' && current == '"') {
                    mode = structure;
                }
            break;
            case commentSingle:
                if(current == '\n') {
                    mode = structure;
                } else {
                    text[i] = ' ';
                }
            break;
            case commentMulti:
                if (prev == '*' && current == '/') {
                    mode = structure;
                }
                if(current != '\n') {
                    text[i] = ' ';
                }
            break;
        }
    }
}

/**
 * Converts newlines in JSON values to breakout newlines.
 * 
 * @param text The text to convert newlines in.
 */
void convertJsonValueNewlinesToBreakout(std::string & text) {
    StripMode mode = structure;
    char prev = 'U', current = 'U';
    for (int i = 0; i < text.size(); i++) {
        prev = current;
        current = text[i];
        switch (mode) {
        case structure:
            if (current == '"') {
                mode = quote;
            }
            break;
        case quote:
            //Check if at end of quote.
            if (prev != '\\' && current == '"') {
                mode = structure;
            } else if (current == '\n') {
                text.erase(i, 1);
                text.insert(i, "\\n");
            }
        }
    }
}

/**
 * Converts breakout newlines in JSON values to newlines.
 * 
 * @param text The text to convert breakout newlines in.
 */
void convertNewlineBreakoutsToNewline(std::string & text) {
    char prev = 'U', current = 'U';
    for (int i = 0; i < text.size(); i++) {
        prev = current;
        current = text[i];
        if (prev == '\\' && current == 'n') {
            text.erase(i - 1, 2);
            text.insert(i - 1, "\n");
            prev = 'U';
            current = text[i];
            i--;
        }
    }
}

/**
 * Converts quotes in JSON values to breakout quotes.
 * 
 * @param text The text to convert quotes in.
 */
void convertQuoteToBreakoutQuote(std::string & text) {
    char prev = 'U', current = 'U';
    for (int i = 0; i < text.size(); i++) {
        prev = current;
        current = text[i];
        if (prev != '\\' && current == '\"') {
            text.insert(i, "\\");
            current = '\\';
        }
    }
}

/**
 * Replaces the first matching character in a string".
 * 
 * @param text The text to replace a character in.
 * @param x The character to replace.
 * @param replacement The string to replace the character with.
 * @return The position of the character replaced, -1 if none.
 */
int replaceFirstOfX(std::string & text, const char x, const std::string replacement) {
    for (int i = 0; i < text.size(); i++) {
        if (text[i] == x) {
            text.erase(i, 1);
            text.insert(i, replacement);
            return i;
        }
    }
    return -1;
}

/**
 * Loads a text file from the path and returns a string.
 * 
 * @param filePath The path to load the file from.
 * @return The file converted into an std::string object.
 */
const std::string fetchText(fs::path filePath) {
    std::ifstream textFile;
    std::stringstream textStream;

    //TODO: Handle read failures more gracefully.
    textFile.open(filePath);
    textStream << textFile.rdbuf();
    textFile.close();

    return textStream.str();
}

/**
 * Loads a JSON file from the path and returns a nlohmann::json object.
 * 
 * @param filePath The path to load the file from.
 * @param valuesHaveNewlines If values have actual newlines in them.
 * @return The JSON file converted into a nlohmann::json object.
 */
const json fetchJson(fs::path filePath, bool valuesHaveNewlines) {
    std::string jsonString = fetchText(filePath);
    stripJsonComments(jsonString);
    if (valuesHaveNewlines) {
        convertJsonValueNewlinesToBreakout(jsonString);
    }
    //TODO: Handle conversion failures more gracefully.
    return json::parse(jsonString);
}

/**
 * Loads a JSON file from the path and returns a nlohmann::json object.
 * 
 * @param filePath The path to load the file from.
 * @return The JSON file converted into a nlohmann::json object.
 */
const json fetchJson(fs::path filePath) {
    return fetchJson(filePath, false);
}

/**
 * Writes a string stream to a specific path.
 * 
 * @param stream The string stream that will be written from.
 * @param filePath The path the file should be written to.
 * @return If the file was written.
 */
const bool writeStringStreamToPath(std::stringstream & stream, std::filesystem::path filePath) {
    if (fs::exists(filePath.parent_path()) || fs::create_directories(filePath.parent_path())) {
        std::ofstream textFile;
        //TODO: Handle write failures more gracefully.
        textFile.open(filePath);
        textFile << stream.rdbuf();
        textFile.close();
        return true;
    }
    return false;
}
