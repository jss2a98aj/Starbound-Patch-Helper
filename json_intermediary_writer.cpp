#include "json_intermediary_writer.h"

#include <string>
#include "utilities.h"

using json = nlohmann::json;

JsonIntermediaryWriter::JsonIntermediaryWriter(){
    
}

int JsonIntermediaryWriter::writeIntermediaryFile(std::stringstream & intermediaryText, FileSettings & fileSettings, const json & sourceJson) {
    totalIntermediaryValues = 0;

    intermediaryText << "{\n";
    //Add patch makers comment if configured.
    if (fileSettings.getAddPatchMakersComment()) {
        intermediaryText << "  //Comment to be added to the top of patches.\n"
            << "  \"patchMakerComment\" : \"\",\n\n";
    }
        
    //iterate if required
    for (const PointerSettings & pointerSettings : fileSettings.getAllPointerSettings()) {
        writeRecursivePointerValuePair(intermediaryText, pointerSettings, sourceJson);
    }

    intermediaryText << "\n}\n";

    return totalIntermediaryValues;
}

bool JsonIntermediaryWriter::writePointerValuePair(std::stringstream & intermediaryText, const PointerSettings & pointerSettings, const json & sourceJson) {
    //Value path as JSON pointer.
    const json::json_pointer valuePointer = json::json_pointer(pointerSettings.path);
    //Value present, copy.
    if (sourceJson.contains(valuePointer)) {
        writePointerValueStart(intermediaryText, pointerSettings);
        intermediaryText << "  \"" << pointerSettings.path << "\" : ";
        if(pointerSettings.convertBreakoutNewlines) {
            std::string sourceJsonText = sourceJson[valuePointer];
            convertNewlineBreakoutsToNewline(sourceJsonText);
            intermediaryText << '"' << sourceJsonText << '"';
        } else {
             intermediaryText << sourceJson[valuePointer];
        }
        //Continue iteration.
        return true;
    //Value missing, use placeholder if possible.
    } else if (pointerSettings.intermediaryPlaceholderCondition == whenPossible) {
        writePointerValueStart(intermediaryText, pointerSettings);
        intermediaryText << "  \"" << pointerSettings.path << "\" : \"\"";
        //Continue iteration.
        return true;
    //Value missing, use placeholder if from exists.
    } else if (pointerSettings.intermediaryPlaceholderCondition == fromExists && pointerSettings.from != "" && sourceJson.contains(json::json_pointer(pointerSettings.from))) {
        writePointerValueStart(intermediaryText, pointerSettings);
        intermediaryText << "  \"" << pointerSettings.path << "\" : \"\"";
        //Continue iteration.
        return true;
    }
    //Stop iteration.
    return false;
}

bool JsonIntermediaryWriter::writeRecursivePointerValuePair(std::stringstream & intermediaryText, const PointerSettings & pointerSettings, const json & sourceJson) {
    //Check if there is a marker configured.
    if (pointerSettings.numericIteratorMarker != "" && (pointerSettings.intermediaryPlaceholderCondition != fromExists || pointerSettings.from != "")) {
        //Check if there is a marker position in path.
        int pathMarkerPosition = pointerSettings.path.find(pointerSettings.numericIteratorMarker);
        //Check if there is a marker position in from.
        int fromMarkerPosition = pointerSettings.from.find(pointerSettings.numericIteratorMarker);
        //If there is a marker to replace in path and, if required, from.
        if (pathMarkerPosition != std::string::npos && (pointerSettings.intermediaryPlaceholderCondition != fromExists || fromMarkerPosition != std::string::npos)) {
            //Pointer settings to be modified for the next recursion.
            PointerSettings modifiedPointerSettings = pointerSettings;
            //Prevent infinite iterations from occuring.
            if(modifiedPointerSettings.intermediaryPlaceholderCondition == whenPossible) {
                modifiedPointerSettings.intermediaryPlaceholderCondition = never;
            }
            //Find the marker position in the intermediary label, if there is one.
            int intermediaryLabelMarkerPosition = pointerSettings.intermediaryLabel.find(pointerSettings.numericIteratorMarker);

            int index = 0;
            //Iterate with increaing index until no writes happen.
            do {
                std::string indexString = std::to_string(index);

                //Replace the first marker in modifiedPath with the current index.
                std::string modifiedPath = pointerSettings.path;
                modifiedPath.erase(pathMarkerPosition, pointerSettings.numericIteratorMarker.length());
                modifiedPath.insert(pathMarkerPosition, indexString);
                modifiedPointerSettings.path = modifiedPath;

                //If this segment of path doesn't exist stop iterating.
                std::string testPath = modifiedPointerSettings.path;
                testPath.erase(pathMarkerPosition + indexString.length());
                if (pointerSettings.intermediaryPlaceholderCondition != fromExists && !sourceJson.contains(json::json_pointer(testPath))) {
                    break;
                }

                //Replace the first marker in modifiedFrom with the currect index if required.
                if (pointerSettings.intermediaryPlaceholderCondition == fromExists) {
                    std::string modifiedFrom = pointerSettings.from;
                    modifiedFrom.erase(fromMarkerPosition, pointerSettings.numericIteratorMarker.length());
                    modifiedFrom.insert(fromMarkerPosition, indexString);
                    modifiedPointerSettings.from = modifiedFrom;

                    //If this segment of from doesn't exist stop iterating.
                    std::string testFrom = modifiedPointerSettings.from;
                    testFrom.erase(fromMarkerPosition + indexString.length());
                    if (!sourceJson.contains(json::json_pointer(testFrom)) && !sourceJson.contains(json::json_pointer(testPath))) {
                        break;
                    }
                }

                //Replace the first marker in modifiedIntermediaryLabel with the current index if it exists.
                if (pointerSettings.intermediaryLabel != "" && intermediaryLabelMarkerPosition != std::string::npos) {
                    std::string modifiedIntermediaryLabel = pointerSettings.intermediaryLabel;
                    modifiedIntermediaryLabel.erase(intermediaryLabelMarkerPosition, pointerSettings.numericIteratorMarker.length());
                    modifiedIntermediaryLabel.insert(intermediaryLabelMarkerPosition, indexString);
                    modifiedPointerSettings.intermediaryLabel = modifiedIntermediaryLabel;
                }

                index++;
            //Recurse and get the result.
            } while (writeRecursivePointerValuePair(intermediaryText, modifiedPointerSettings, sourceJson));
            return true;
        }
    }
    //There is no marker to replace.
    return writePointerValuePair(intermediaryText, pointerSettings, sourceJson);
}

void JsonIntermediaryWriter::writePointerValueStart(std::stringstream & intermediaryText, const PointerSettings & pointerSettings) {
    if (totalIntermediaryValues > 0) intermediaryText << ",\n\n";
    if (pointerSettings.intermediaryLabel != "") intermediaryText << "  //" + pointerSettings.intermediaryLabel + '\n';
    totalIntermediaryValues++;
}
