#include "json_patch_writer.h"

#include "utilities.h"

using json = nlohmann::json;

JsonPatchWriter::JsonPatchWriter(MasterSettings masterSettings) {
    baselinePatchStyle = masterSettings.baselinePatchStyle;
    useInverseTestOps = masterSettings.getUseInverseTestOps();
    useOperationSets = masterSettings.getUseOperationSets();
}

/**
 * Writes a patch file. Some configurations will not comply with official JSON patch standards.
 * 
 * @param patchText The string stream the patches will be written to.
 * @param fileSettings The file extension specific settings to use when making patches.
 * @param sourceJson The JSON patches will assume is used as a base.
 * @param intermediaryJson The JSON patches will try to make the base mimic when applied.
 * @return How many values the resulting patch will add or replace.
 */
int JsonPatchWriter::writePatchFile(std::stringstream & patchText, FileSettings & fileSettings, const json & sourceJson, const json & intermediaryJson) {
    indentModifier = baselinePatchStyle.getIndentationInOuterBrackets() ? 1 : 0;
    currentOps = 0;
    currentOpSets = 0;

    //Add patch makers comment if configured.
    if (fileSettings.getAddPatchMakersComment() && intermediaryJson.contains("patchMakerComment")) {
        std::string comment = intermediaryJson["patchMakerComment"];
        if (comment != "") {
            patchText << "//" << comment << std::endl;
        }
    }
    //Opening outer bracket
    patchText << '[';
    if (baselinePatchStyle.getNewLineAfterOuterOpenerBracket()) patchText << std::endl;

    //TODO: if only one op set in a patch skip set brackets (configurable)
    //Write operation sets
    for (const PointerSettings & pointerSettings : fileSettings.getAllPointerSettings()) {
        if (!pointerSettings.reference) {
            writeRecursiveOperationSet(patchText, pointerSettings, sourceJson, intermediaryJson);
        }
    }

    //New line after the last operation set closer.
    if (useOperationSets && (baselinePatchStyle.getNewLineAfterOperationSetCloserBracket() || baselinePatchStyle.getNewLineAfterOperationSetCloserBracketComma())) patchText << std::endl;

    //Closing outer bracket
    patchText << ']';
    if (baselinePatchStyle.getNewLineAfterOuterCloserBracket()) patchText << std::endl;

    return currentOpSets;
}

bool JsonPatchWriter::writeOperationSet(std::stringstream & patchText, const PointerSettings & pointerSettings, const json & sourceJson, const json & intermediaryJson) {
    //Value path as JSON pointer.
    const json::json_pointer valuePointer = json::json_pointer(pointerSettings.path);

    //Intermediary value found.
    if (intermediaryJson.contains(pointerSettings.path)) {
        //Intermediary value is placeholder.
        if (intermediaryJson[pointerSettings.path] == "") {
            //Operation if placeholder is not none and source value not found.
            if (pointerSettings.patchOperationIfPlaceholder == none || !sourceJson.contains(valuePointer)) {
                //Patch operation if placeholder set to copy.
		        if (pointerSettings.patchOperationIfPlaceholder == copy) {
                    writeOperationSetOpener(patchText);

                    if (pointerSettings.patchTestOperation && useInverseTestOps) {
                        writeOperation(patchText, pointerSettings.from, "false", testInverse);
                        
                        writeOperation(patchText, pointerSettings.path, "true", testInverse);
                    }

                    writeOperation(patchText, pointerSettings.from, pointerSettings.path, copyValue);

                    writeOperationSetCloser(patchText);
                //Patch operation if placeholder set to move.
                } else if (pointerSettings.patchOperationIfPlaceholder == move) {
                    writeOperationSetOpener(patchText);

                    if (pointerSettings.patchTestOperation && useInverseTestOps) {
                        writeOperation(patchText, pointerSettings.from, "false", testInverse);

                        writeOperation(patchText, pointerSettings.path, "true", testInverse);
                    }

                    writeOperation(patchText, pointerSettings.from, pointerSettings.path, moveValue);

                    writeOperationSetCloser(patchText);
                }
            }
        //Intermediary value is not placeholder
        } else {
            //Source value found.
            if (sourceJson.contains(valuePointer)) {
                //Remove if equals is not blank and the source value matches it.
                if (pointerSettings.patchRemoveIfEquals != "" && sourceJson[valuePointer].dump() == pointerSettings.patchRemoveIfEquals) {
                    writeOperationSetOpener(patchText);

                    if (pointerSettings.patchTestOperation) {
                        std::string sourceValueContents = sourceJson[valuePointer].dump();
                        if (pointerSettings.convertBreakoutNewlines) convertNewlineBreakoutsToNewline(sourceValueContents);
                        writeOperation(patchText, pointerSettings.path, sourceValueContents, testValue);
                    }

                    writeOperation(patchText, pointerSettings.path, "", removeValue);

                    writeOperationSetCloser(patchText);
                //Intermediary value different from source value.
                } else if (intermediaryJson[pointerSettings.path] != sourceJson[valuePointer]) {
                    writeOperationSetOpener(patchText);

                    if (pointerSettings.patchTestOperation) {
                        std::string sourceValueContents = sourceJson[valuePointer].dump();
                        if (pointerSettings.convertBreakoutNewlines) convertNewlineBreakoutsToNewline(sourceValueContents);
                        writeOperation(patchText, pointerSettings.path, sourceValueContents, testValue);
                    }

                    std::string intermediaryValueContents = intermediaryJson[pointerSettings.path].dump();
                    if (pointerSettings.convertBreakoutNewlines) convertNewlineBreakoutsToNewline(intermediaryValueContents);
                    writeOperation(patchText, pointerSettings.path, intermediaryValueContents, replaceValue);

                    writeOperationSetCloser(patchText);
                }
            //Source value not found.
            } else {
                writeOperationSetOpener(patchText);

                if (pointerSettings.patchTestOperation && useInverseTestOps) writeOperation(patchText, pointerSettings.path, "true", testInverse);

                std::string intermediaryValueContents = intermediaryJson[pointerSettings.path].dump();
                if (pointerSettings.convertBreakoutNewlines) convertNewlineBreakoutsToNewline(intermediaryValueContents);
                writeOperation(patchText, pointerSettings.path, intermediaryValueContents, addValue);

                writeOperationSetCloser(patchText);
            }
        }
        //Continue iteration.
        return true;
    }
    //Stop iteration.
    return false;
}

bool JsonPatchWriter::writeRecursiveOperationSet(std::stringstream & patchText, const PointerSettings & pointerSettings, const json & sourceJson, const json & intermediaryJson) {
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

            int index = 0;
            //Iterate with increasing index until no writes happen.
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

                //Replace the first marker in modifiedFrom with the current index if required.
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

                index++;
            //Recurse and get the result.
            } while (writeRecursiveOperationSet(patchText, modifiedPointerSettings, sourceJson, intermediaryJson));
            return true;
        }
    }
    //There is no marker to replace.
    return writeOperationSet(patchText, pointerSettings, sourceJson, intermediaryJson);
}

void JsonPatchWriter::writeOperation(std::stringstream & patchText, std::string path, std::string value, patchOperation operation) {
    //Comma and new line after all but the last patch.
    if (currentOps > 0) {
        if (baselinePatchStyle.getNewLineAfterOperationCloserBracket()) patchText << std::endl;
        patchText << ',';
        if (baselinePatchStyle.getNewLineAfterOperationCloserBracketComma()) patchText << std::endl;
    }

    //Opening patch bracket.
    writeIndent(patchText);
 
    patchText << '{';
    if (baselinePatchStyle.getNewLineAfterOperationOpenerBracket()) patchText << std::endl;
    indentModifier += baselinePatchStyle.getIndentationInOperationBrackets() ? 1 : 0;

    //op
    writeIndent(patchText);
    patchText << "\"op\"";
    writeColon(patchText);
    if (operation == addValue) {
        patchText << "\"add\",";
    } else if (operation == replaceValue) {
        patchText << "\"replace\",";
    } else if (operation == removeValue) {
        patchText << "\"remove\",";
    } else if (operation == copyValue) {
        patchText << "\"copy\",";
    } else if (operation == moveValue) {
        patchText << "\"move\",";
    } else if (operation == testValue || operation == testInverse) {
        patchText << "\"test\",";
    }
    if (baselinePatchStyle.getNewLineAfterOperationSegment()) patchText << std::endl;

    //copy/move from, other path.
    writeIndent(patchText);
    if (operation == copyValue || operation == moveValue) {
        patchText << "\"from\"";
    } else {
        patchText << "\"path\"";
    }
    writeColon(patchText);
    patchText << '\"' << path << '\"';

    //Write third operation segment if not remove.
    if (operation != removeValue) {
        patchText << ',';
        if (baselinePatchStyle.getNewLineAfterOperationSegment()) patchText << std::endl;

        //copy/move path, inverse test inverse, other value.
        writeIndent(patchText);
        if (operation == copyValue || operation == moveValue) {
            patchText << "\"path\"";
        } else if (operation == testInverse)  {
            patchText << "\"inverse\"";
        } else {
            patchText << "\"value\"";
        }
        writeColon(patchText);
        if (operation == copyValue || operation == moveValue) {
            patchText << '\"' << value << '\"';
        } else {
            patchText << value;
        }
        
    }

    if (baselinePatchStyle.getNewLineAfterOperationSegment()) patchText << std::endl;

    //Closing patch bracket.
    indentModifier -= baselinePatchStyle.getIndentationInOperationBrackets() ? 1 : 0;
    writeIndent(patchText);
    patchText << '}';
    currentOps++;
}

void JsonPatchWriter::writeOperationSetOpener(std::stringstream & patchText) {
    if (useOperationSets) {
        currentOps = 0;
        //Comma and new line after all but the last patch set.
        if (currentOpSets > 0) {
            if (baselinePatchStyle.getNewLineAfterOperationSetCloserBracket()) patchText << std::endl;
            patchText << ',';
            if (baselinePatchStyle.getNewLineAfterOperationSetCloserBracketComma()) patchText << std::endl;
        }

        //Opening patch set bracket
        writeIndent(patchText);
        patchText << '[';
        if (baselinePatchStyle.getNewLineAfterOperationSetOpenerBracket()) patchText << std::endl;
        indentModifier += baselinePatchStyle.getIndentationInOperationSetBrackets() ? 1 : 0;
    }
}

void JsonPatchWriter::writeOperationSetCloser(std::stringstream & patchText) {
    if (useOperationSets) {
        //New line after the last patch.
        if (baselinePatchStyle.getNewLineAfterOperationCloserBracket() || baselinePatchStyle.getNewLineAfterOperationCloserBracketComma()) patchText << std::endl;
        //Closing patch set bracket
        indentModifier -= baselinePatchStyle.getIndentationInOperationSetBrackets() ? 1 : 0;
        writeIndent(patchText);
        patchText << ']';
    }
    currentOpSets++;
}

void JsonPatchWriter::writeIndent(std::stringstream & patchText) {
    const int indentBy = indentModifier * baselinePatchStyle.getIndentationSpacesPerModifier();
    for (int i = 0; i < indentBy; i++) {
        patchText << ' ';
    }
}

void JsonPatchWriter::writeColon(std::stringstream & patchText) {
    if (baselinePatchStyle.getSpaceBeforeOperationColon()) patchText << ' ';
    patchText << ':';
    if (baselinePatchStyle.getSpaceAfterOperationColon()) patchText << ' ';
}
