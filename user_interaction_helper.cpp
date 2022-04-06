#include "user_interaction_helper.h"

#include <iostream>
#include <limits>

namespace fs = std::filesystem;

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

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

bool warnIfNothingAtPath(std::filesystem::path filePath, std::string fileDescription) {
    if (!fs::exists(filePath)) {
        std::cout << "The " << fileDescription << " folder is missing.\n"
            << filePath.string()
            << "\nThere is nothing to do, aborting.\n";
        return true;
    }
    return false;
}
