#include "user_interaction_helper.h"

#include <iostream>
#include <limits>

namespace fs = std::filesystem;

/**
 * Clears the input stream.
 */
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * Prompts for a yes/no input from the terminal.
 * 
 * @param message  The message to display.
 * @return If the user answered yes or no.
 */
bool requestBoolean(std::string message) {
    do {
        char input;
        std::cout << message << " (y/n):";
        if (std::cin >> input) {
            clearInput();
            switch (toupper(input)) {
            case 'Y':
                return true;
            case 'N':
                return false;
            default:
                std::cout << "Invalid input.\n" << message << " (y/n):";
            }
        }
    } while (true);
}

/**
 * Checks if the file at the specified path exists and issues a warning if not.
 * 
 * @param message The message to display.
 * @return If a warning was issued.
 */
bool warnIfNothingAtPath(std::filesystem::path filePath, std::string fileDescription) {
    if (!fs::exists(filePath)) {
        std::cout << "The " << fileDescription << " folder is missing.\n"
            << filePath.string()
            << "\nThere is nothing to do, aborting.\n";
        return true;
    }
    return false;
}
