#include "utilities.h"

#include <fstream>

using json = nlohmann::json;

namespace fs = std::filesystem;

void stripJsonComments(std::string & text) {
    char mode = 'D';
    int commentStart, commentLength;
    char prev = 'U', current = 'U';
    for (int i = 0; i < text.size(); i++) {
        prev = current;
        current = text[i];

        switch (mode) {
        //Default state.
        case 'D':
            //Check for a quote.
            if (current == '"') {
                mode = 'Q';
            }
            //Check for single line comment.
            else if (prev == '/' && current == '/') {
                commentStart = i - 1;
                commentLength = 2;
                mode = 'S';
            //Check for multi line comment.
            } else if (prev == '/' && current == '*') {
                commentStart = i - 1;
                commentLength = 2;
                mode = 'M';
            }
            break;
        //Single line comment state.
        case 'S':
            //Check if at end of comment.
            if (current == '\n') {
                text.erase(commentStart, commentLength);
                //Move the iterator back to where the comment started.
                i = commentStart;
                prev = i > 0 ? text[i - 1] : 'U';
                mode = 'D';
            } else {
                commentLength++;
            }
            break;
        //Multi line comment state.
        case 'M':
            //Check if at end of comment.
            if (prev == '*' && current == '/') {
                text.erase(commentStart, commentLength + 1);
                //Move the iterator back to where the comment started.
                i = commentStart;
                prev = i > 0 ? text[i - 1] : 'U';
                mode = 'D';
            } else {
                commentLength++;
            }
            break;
        //Quote state.
        case 'Q':
            //Check if at end of quote.
            if (prev != '\\' && current == '"') {
                mode = 'D';
            }
        }
    }
    //If in a comment mode after exiting the loop clean it up.
    if (mode == 'S' || mode == 'M') {
        text.erase(commentStart, commentLength - 1);
    }
}

void convertJsonValueNewlinesToBreakout(std::string & text) {
    char mode = 'D';
    char prev = 'U', current = 'U';
    for (int i = 0; i < text.size(); i++) {
        prev = current;
        current = text[i];
        switch (mode) {
        //Default state.
        case 'D':
            if (current == '"') {
                mode = 'Q';
            }
            break;
        //Quote state.
        case 'Q':
            //Check if at end of quote.
            if (prev != '\\' && current == '"') {
                mode = 'D';
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
            text.insert(i, "\n");
        } else if (prev != '\\' && current == '\"') {
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
