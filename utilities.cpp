#include "utilities.h"

#include <fstream>

using json = nlohmann::json;

namespace fs = std::filesystem;

enum StripMode { structure, quote, commentSingle, commentMulti };

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

const std::string fetchText(fs::path filePath) {
    std::ifstream textFile;
    std::stringstream textStream;

    //TODO: Handle read failures more gracefully.
    textFile.open(filePath);
    textStream << textFile.rdbuf();
    textFile.close();

    return textStream.str();
}

const json fetchJson(fs::path filePath, bool valuesHaveNewlines) {
    std::string jsonString = fetchText(filePath);
    stripJsonComments(jsonString);
    if (valuesHaveNewlines) {
        convertJsonValueNewlinesToBreakout(jsonString);
    }
    //TODO: Handle conversion failures more gracefully.
    return json::parse(jsonString);
}

const json fetchJson(fs::path filePath) {
    return fetchJson(filePath, false);
}

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
