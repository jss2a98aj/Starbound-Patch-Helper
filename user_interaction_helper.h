#pragma once

#include <filesystem>

void clearInput();

bool requestBoolean(std::string message);

bool warnIfNothingAtPath(std::filesystem::path filePath, std::string fileDescription);
