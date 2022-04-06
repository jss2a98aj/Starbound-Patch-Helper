#pragma once

#include <filesystem>

/**
 * Clears the input stream.
 */
void clearInput();

/**
 * Promts for a yes/no input from the terminal.
 * 
 * @param message  The message to display.
 * @return If the user answered yes or no.
 */
bool requestBoolean(std::string message);

/**
 * Checks if the file at the specified path exists and issues a warning if not.
 * 
 * @param message The message to display.
 * @return If a warning was issued.
 */
bool warnIfNothingAtPath(std::filesystem::path filePath, std::string fileDescription);
