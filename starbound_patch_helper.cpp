#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "global_settings.h"
#include "json_intermediary_writer.h"
#include "json_patch_writer.h"
#include "parse_settings.h"
#include "user_interaction_helper.h"
#include "utilities.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

void parseAssets(MasterSettings & masterSettings, const fs::path sourceAssetPath, const fs::path intermediaryAssetPath, std::vector<FileSettings> fileSettings);
void makePatches(MasterSettings & masterSettings, const fs::path sourceAssetPath, const fs::path intermediaryAssetPath, const fs::path patchOutputPath, std::vector<FileSettings> fileSettings);

int main(int argc, char * argv[]) {
    
    const std::string strQuit = "quit";
    const std::string strHelp = "help";
    const std::string strParse = "parse";
    const std::string strMakePatches = "makepatches";
    const std::string strOverwrite = "overwrite";

    //Paths that will be used for various things.
    const fs::path parseSettingsPath = fs::current_path() /= "config/parse_targets";
    fs::path sourceAssetPath;
    fs::path intermediaryAssetPath;
    fs::path patchOutputPath;

    MasterSettings masterSettings = MasterSettings(fs::current_path() /= "config/settings.json");
    std::cout << "Selected patch style: " << masterSettings.getBaselinePatchStyleName() << std::endl;

    //Populate parse settings for every configured file type.
    std::vector<FileSettings> allFileSettings;
    for (const auto & directory : fs::recursive_directory_iterator(parseSettingsPath)) {
        if (directory.path().has_extension() && directory.path().extension().string() == ".json") {
            FileSettings fileSettings = FileSettings(directory);
            if (fileSettings.hasPointerSettings()) {
                allFileSettings.push_back(fileSettings);
            } else {
                std::cout << "Parse target config with no valid values at:\n"
                << directory.path().string()
                << "\nParse target will be ignored.";
            }
        }
    }

    //If parameters are used then never prompt for user inputs.
    //TODO: Support path params
    if (argc > 1) {
        //Help.
        if (argv[1] == strHelp) {
            std::cout << "Possible parameters:\n"
                << strParse //<< " [source asset path] [intermediary asset path]"
                << "\n    Parses content from source assets into intermediary assets.\n"
                << strMakePatches //<< " [source asset path] [intermediary asset path] [patch output path]"
                << "\n    Uses source assets and modified intermediary assets to produce patches.\n";
        //Parse.
        } else if (argv[1] == strParse) {
            sourceAssetPath = fs::current_path() /= "source_assets";
            intermediaryAssetPath = fs::current_path() /= "intermediary_assets";

            parseAssets(masterSettings, sourceAssetPath, intermediaryAssetPath, allFileSettings);
        //Make patches.
        } else if (argv[1] == strMakePatches) {
            sourceAssetPath = fs::current_path() /= "source_assets";
            intermediaryAssetPath = fs::current_path() /= "intermediary_assets";
            patchOutputPath = fs::current_path() /= "patch_output";

            makePatches(masterSettings, sourceAssetPath, intermediaryAssetPath, patchOutputPath, allFileSettings);
        //Invalid command.
        } else {
            std::cout << "Invalid command:\n"
                << argv[1] << std::endl;
            return 1;
        }
    //If parameters are not used prompt for user inputs.
    } else {
        //Setup paths.
        sourceAssetPath = fs::current_path() /= "source_assets";
        intermediaryAssetPath = fs::current_path() /= "intermediary_assets";
        patchOutputPath = fs::current_path() /= "patch_output";

        //Prompt for input until quit token is encountered.
        bool quit = false;
        do {
            std::cout << "Options:\n"
                << "\"" << strParse << "\" parse source assets into intermediary assets.\n"
                << "\"" << strMakePatches << "\" produce patches using source assets and intermediary assets.\n"
                << "\"" << strQuit << "\" exit the program.\n";
            //Get input.
            std::string input;
            if (std::cin >> input) {
                clearInput();
                //Parse assets
                if (input == strParse) {
                    //Ensure the source asset folder exists.
                    if (!fs::exists(sourceAssetPath)) {
                        fs::create_directory(sourceAssetPath);
                        std::cout << "Source asset folder at:\n"
                            << sourceAssetPath.string()
                            << "\nCopy assets that should be parsed there and then proceed.\n";
                        system("pause");
                    }
                    //If an intermediary asset folder exists prompt the user before deleting it.
                    if (fs::exists(intermediaryAssetPath)) {
                        if (requestBoolean("An intermediary asset folder already exists.\nShould it be replaced?")) {
                            std::cout << "Deleting old intermediary asset folder.\n";
                            fs::remove_all(intermediaryAssetPath);
                            std::cout << "Old intermediary asset folder deleted.\n";
                        } else {
                            std::cout << "Aborting parse.\n";
                            break;
                        }
                    }
                    parseAssets(masterSettings, sourceAssetPath, intermediaryAssetPath, allFileSettings);
                //Make patches
                } else if (input == strMakePatches) {
                    //If a patch asset folder exists prompt the user before deleting it.
                    if (fs::exists(patchOutputPath)) {
                        if (requestBoolean("A patch folder already exists.\nShould it be replaced?")) {
                            std::cout << "Deleting old patch output folder.\n";
                            fs::remove_all(patchOutputPath);
                            std::cout << "Old patch patch output folder deleted.\n";
                        } else {
                            std::cout << "Aborting make patches.\n";
                            break;
                        }
                    }
                    makePatches(masterSettings, sourceAssetPath, intermediaryAssetPath, patchOutputPath, allFileSettings);
                //Quit
                } else if (input == strQuit) {
                    quit = true;
                //Bad input
                } else {
                    std::cout << "Invalid input, try again.\n";
                }
            }
        } while (!quit);
    }

    return 0;
}

void parseAssets(MasterSettings & masterSettings, const fs::path sourceAssetPath, const fs::path intermediaryAssetPath, std::vector<FileSettings> allFileSettings) {
    //Stop if the source asset folder does not exist.
    if (warnIfNothingAtPath(sourceAssetPath, "source asset")) return;

    //Stop if the intermediary asset folder exists unless in overwrite mode.
    if (fs::exists(intermediaryAssetPath)) {
        if (masterSettings.getOverwriteFiles()) {
            std::cout << "Deleting old intermediary asset folder.\n";
            fs::remove_all(intermediaryAssetPath);
            std::cout << "Old intermediary asset folder deleted.\n";
        } else {
            std::cout << "Intermediary asset folder already exists at:"
                << intermediaryAssetPath.string()
                << "\nNo files will be written.\n"
                << "Delete the folder or run again in overwrite mode.\n";
            return;
        }
    }

    std::cout << "Making intermediary files.\n";

    auto startTime = std::chrono::high_resolution_clock::now();
    int totalIntermediaryFilesMade = 0;
    JsonIntermediaryWriter intermediaryWriter = JsonIntermediaryWriter();
    //Iteratively parse source assets.
    for (const auto & directory : fs::recursive_directory_iterator(sourceAssetPath)) {
        if (directory.path().has_extension()) {

            //Check if the extension is in allFileSettings.
            const std::string extension = directory.path().extension().string();
            for (FileSettings & fileSettings : allFileSettings) {
                if (extension == fileSettings.getFileExtension()) {
                    //Source JSON.
                    const json sourceJson = fetchJson(directory, fileSettings.getValuesContainNewlines());

                    std::stringstream intermediaryText;
                    int currentValuesToKeep = intermediaryWriter.writeIntermediaryFile(intermediaryText, fileSettings, sourceJson);

                    //If there were any values to keep save the file.
                    if (currentValuesToKeep > 0) {
                        //Where the intermediary asset should go.
                        fs::path intermediaryPath = intermediaryAssetPath;
                        std::string pathFragment = directory.path().string();
                        pathFragment.erase(0, sourceAssetPath.string().length());
                        intermediaryPath += pathFragment;

                        //Write the file
                        if (!writeStringStreamToPath(intermediaryText, intermediaryPath)) {
                            std::cout << "Failed to write intermediary file to:\n"
                                << intermediaryPath.string() << std::endl;
                                
                        }

                        totalIntermediaryFilesMade++;
                    }
                    //There can only be one match.
                    break;
                }
            }
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime);
    //Finished parse notification
    std::cout << totalIntermediaryFilesMade << " intermediary files created in " << duration.count() << "s at:\n"
        << intermediaryAssetPath.string() << std::endl;
}

void makePatches(MasterSettings & masterSettings, const fs::path sourceAssetPath, const fs::path intermediaryAssetPath, const fs::path patchOutputPath, std::vector<FileSettings> allFileSettings) {
    //Stop if the source asset folder does not exist.
    if (warnIfNothingAtPath(sourceAssetPath, "source asset")) return;
    //Stop if the intermediary asset folder does not exist.
    if (warnIfNothingAtPath(intermediaryAssetPath, "intermediary asset")) return;

    //Stop if the patch output folder exists unless in overwrite mode.
    if (fs::exists(patchOutputPath)) {
        if (masterSettings.getOverwriteFiles()) {
            std::cout << "Deleting old patch output folder.\n";
            fs::remove_all(patchOutputPath);
            std::cout << "Old patch output folder deleted.\n";
        } else {
            std::cout << "Patch output folder already exists at:"
                << patchOutputPath.string()
                << "\nNo files will be written.\n"
                << "Delete the folder or run again in overwrite mode.\n";
            return;
        }
    }

    std::cout << "Making patches.\n";

    auto startTime = std::chrono::high_resolution_clock::now();
    int totalPatchesMade = 0;
    int totalValuesAltered = 0;
    JsonPatchWriter patchWriter = JsonPatchWriter(masterSettings);

    //Iteratively generate patches.
    for (const auto & directory : fs::recursive_directory_iterator(intermediaryAssetPath)) {
        //Folders should be ignored.
        if (directory.path().has_extension()) {
            //Get the file extension.
            const std::string extension = directory.path().extension().string();
            //Check if the extension should be read.
            for (FileSettings & fileSettings : allFileSettings) {
                if (extension == fileSettings.getFileExtension()) {
                    const json intermediaryJson = fetchJson(directory, fileSettings.getValuesContainNewlines());

                    //Get the source file path.
                    fs::path sourceJsonPath = sourceAssetPath;
                    std::string pathFragment = directory.path().string();
                    pathFragment.erase(0, intermediaryAssetPath.string().length());
                    sourceJsonPath += pathFragment;

                    //If the source version does not exists there is nothing to do. It should if the user did not delete it.
                    if (fs::exists(sourceJsonPath)) {
                        //Source JSON.
                        const json sourceJson = fetchJson(sourceJsonPath, fileSettings.getValuesContainNewlines());

                        std::stringstream patchText;
                        int currentOps = 0;

                        //Write the patch JSON text.
                        currentOps = patchWriter.writePatchFile(patchText, fileSettings, sourceJson, intermediaryJson);
                        totalValuesAltered += currentOps;

                        //If there were any ops save the file.
                        if (currentOps > 0) {
                            //Get the patch file path.
                            fs::path patchFilePath = patchOutputPath;
                            patchFilePath += pathFragment;
                            patchFilePath += ".patch";

                            //Creating the patch file.
                            if(!writeStringStreamToPath(patchText, patchFilePath)) {
                                std::cout << "Failed to write patch file to:\n"
                                    << patchFilePath.string() << std::endl;
                            }

                            totalPatchesMade++;
                        }
                    } else {
                        std::cout << "Source asset \"" << pathFragment << "\" not found, skipping patch.\n";
                    }
                }
            }
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime);
    //Finished patch output notification
    std::cout << totalPatchesMade << " patches containing " << totalValuesAltered << " operation sets created in " << duration.count() << "s at:\n"
        << patchOutputPath.string() << std::endl;
}
