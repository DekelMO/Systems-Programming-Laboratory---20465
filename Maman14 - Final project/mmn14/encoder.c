#include "encoder.h"

const char *REGISTERS_NAMES[] = {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
const char *REGISTERS_ADDRESS_NAMES[] = {
        "*r0", "*r1", "*r2", "*r3", "*r4", "*r5", "*r6", "*r7"};

int first_scan(char *fileName, FILE *file, FILE *writeFile, FILE *writeDataFile, hashTableInt *table, int *IC, int *DC,
               hashTableInt *entriesTable, hashTableInt *externsTable, int *lines) {
    int i, j, startParamsIdx;
    /* flag whether to encode currentLine of not */
    int encodeLine = 1, continueToSecondScan = 1;
    /* counting starts from 1 */
    int lineNum = 0;
    /* reading line from file */
    char currentLine[MAX_WORD_LENGTH];
    char copyCurrentLine[MAX_WORD_LENGTH];
    char operationString[MAX_WORD_LENGTH];
    char *token;
    /* label to store in symbol's table (as key) */
    char *labelName = NULL;
    char storeDataString[STRING_STORE_COMMAND_LENGTH]; /* strlen(".string") = 7 */
    int currentLabelLength, maxLabelLength = -1;
    int insertLabelReturnCode;
    /* for iterating through linked list in hash table incrementing each node by IC */
    hashTableIntItem *current;
    /* current line number of words */
    int L;
    /* encoded string */
    char encodedString[NUM_BITS];
    short operationEncode = 0;
    int areEntries = 0;
    FILE *entriesFile;
    char *newEntryFileName = NULL;
    int labelArraySize = DEF_ARRAY_SIZE;
    const char entryFilePostfix[] = ".ent";
    int labelsInArray = 0;
    char **labelsArray = labelsArray = (char **) malloc(labelArraySize * sizeof(char *));
    int fileNameLength;
    fileNameLength =(int) strlen(fileName);

    if (labelsArray == NULL) {
        fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
        return MEMORY_NOT_ALLOCATED_ERROR_CODE;
    }

    while (fgets(currentLine, MAX_WORD_LENGTH, file) != NULL) {
        L = 0;
        lineNum++;
        if (currentLine[0] == '\n') return continueToSecondScan;
        if (strlen(currentLine) >= 1) {
            if (currentLine[strlen(currentLine) - 1] == '\n') currentLine[strlen(currentLine) - 1] = '\0';
        } else continue;

        strcpy(copyCurrentLine, currentLine);
        L = validLine(copyCurrentLine);
        strcpy(copyCurrentLine, currentLine);
        currentLabelLength = 0;
        *lines += L;
        if (!L) {
            fprintf(stderr, "in line %d\n\n", lineNum);
            encodeLine = 0;
            continueToSecondScan = 0;
        }

        if (!encodeLine) {
            encodeLine = 1;
            continue;
        }

        if (command_code(currentLine) == RTS_CODE || command_code(currentLine) == STOP_CODE) {
            encode_regular_command(writeFile, command_code(currentLine),
                                   currentLine, lineNum, encodedString);
            (*IC) += L;
            continue;
        }

        /* no label declared */
        if (!strstr(currentLine, ":")) {
            /* labels declared as .entry/extern commands are irrelevant */
            if (strstr(currentLine, ENTRY_COMMAND)) {
                i = (int) strlen(ENTRY_COMMAND);
                for (; isspace(currentLine[i]); i++); /* skip spaces */
                for (j = i; !isspace(currentLine[j]) && currentLine[j] != '\0'; j++); /* finding label name */
                currentLabelLength = j - i;
                /* allocating memory */
                if (labelName == NULL) {
                    maxLabelLength = currentLabelLength;
                    labelName = (char *) malloc(maxLabelLength);
                } else if (maxLabelLength < currentLabelLength) {
                    maxLabelLength = currentLabelLength;
                    labelName = (char *) realloc(labelName, maxLabelLength);
                }
                /* memory allocation was unsuccessful */
                if (labelName == NULL) {
                    fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
                    return MEMORY_NOT_ALLOCATED_ERROR_CODE;
                }
                /* initializing labelName */
                strncpy(labelName, &currentLine[j - currentLabelLength], currentLabelLength);
                labelName[currentLabelLength] = '\0';
                /* handle duplications and handling case where label was declared before ".entry labelName" command */
                if (contains_key_int(table, labelName)) {
                    /* setting isData to -1 means no change for initial isData value */
                    insertLabelReturnCode = insert_int(entriesTable, labelName, get_value_int(table, labelName), -1);
                    if (insertLabelReturnCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                        fprintf(stderr, "Label %s already exists!\n", labelName);
                        continueToSecondScan = 0;
                        continue;
                    }
                    /* label already declared */
                } else if (contains_key_int(entriesTable, labelName)) {
                    fprintf(stderr, "%s: %s", labelName, LABEL_DECLARED_ENTRY_ERROR_MESSAGE);
                    continueToSecondScan = 0;
                    continue;
                } else if (contains_key_int(externsTable, labelName)) {
                    fprintf(stderr, "%s: %s", labelName, LABEL_DECLARED_EXTERN_AND_ENTRY_ERROR_MESSAGE);
                    continueToSecondScan = 0;
                    continue;
                } else {
                    insertLabelReturnCode = insert_int(entriesTable, labelName, *IC, 0);
                    if (insertLabelReturnCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                        fprintf(stderr, "Label %s already exists!\n", labelName);
                        continueToSecondScan = 0;
                        continue;
                    }
                }

                if (maxLabelLength != -1) memset(labelName, 0, maxLabelLength);
                memset(currentLine, 0, MAX_WORD_LENGTH);
                memset(copyCurrentLine, 0, MAX_WORD_LENGTH);

                continue;
            } else if (strstr(currentLine, EXTERN_COMMAND)) {
                i = (int) strlen(EXTERN_COMMAND);
                for (; isspace(currentLine[i]); i++); /* skip spaces */
                for (j = i; !isspace(currentLine[j]) && currentLine[j] != '\0'; j++); /* finding label name */
                currentLabelLength = j - i;
                /* allocating memory */
                if (labelName == NULL) {
                    maxLabelLength = currentLabelLength;
                    labelName = (char *) calloc(maxLabelLength, sizeof(char));
                } else if (maxLabelLength < currentLabelLength) {
                    maxLabelLength = currentLabelLength;
                    labelName = (char *) realloc(labelName, maxLabelLength);
                }
                /* memory allocation was unsuccessful */
                if (labelName == NULL) {
                    fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
                    return MEMORY_NOT_ALLOCATED_ERROR_CODE;
                }
                /* initializing labelName */
                strncpy(labelName, &currentLine[j - currentLabelLength], currentLabelLength);
                labelName[currentLabelLength] = '\0';

                /* handle duplications */
                if (contains_key_int(externsTable, labelName)) {
                    fprintf(stderr, "%s: %s", labelName, LABEL_DECLARED_EXTERN_ERROR_MESSAGE);
                    continueToSecondScan = 0;
                    continue;
                } else if (contains_key_int(entriesTable, labelName)) {
                    fprintf(stderr, "%s: %s", labelName, LABEL_DECLARED_ENTRY_AND_EXTERN_ERROR_MESSAGE);
                    continueToSecondScan = 0;
                    continue;
                } else if (contains_key_int(table, labelName)) {
                    fprintf(stderr, "%s: %s", labelName, REDEFINITION_OF_LABEL_ERROR_MESSAGE);
                    continueToSecondScan = 0;
                    continue;
                } else {
                    insertLabelReturnCode = insert_int(externsTable, labelName, 1, 0);
                    if (insertLabelReturnCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                        fprintf(stderr, "Label %s already exists!\n", labelName);
                        continueToSecondScan = 0;
                        continue;
                    }
                }

                if (maxLabelLength != -1) memset(labelName, 0, maxLabelLength);
                memset(currentLine, 0, MAX_WORD_LENGTH);
                memset(copyCurrentLine, 0, MAX_WORD_LENGTH);

                continue;
            }


            if (strstr(currentLine, STRING_STORE_COMMAND)) {
                strcpy(copyCurrentLine, currentLine);
                /* reaching .string declaration */
                for (startParamsIdx = 0; currentLine[startParamsIdx] != '.'; startParamsIdx++);
                /* skip string word and spaces after */
                for (startParamsIdx = startParamsIdx + 1; isalpha(currentLine[startParamsIdx])
                                                          || isspace(currentLine[startParamsIdx]); startParamsIdx++);
                startParamsIdx++; /* skip \" */
                encode_data_command(writeDataFile, &copyCurrentLine[startParamsIdx], 0, lineNum, encodedString, DC);
                continue;
            } else if (strstr(currentLine, DATA_STORE_COMMAND)) {
                strcpy(copyCurrentLine, currentLine);
                /* reaching .data declaration */
                for (startParamsIdx = 0; currentLine[startParamsIdx] != '.'; startParamsIdx++);
                /* skip data word and spaces after */
                for (startParamsIdx = startParamsIdx + 1; isalpha(currentLine[startParamsIdx])
                                                          || isspace(currentLine[startParamsIdx]); startParamsIdx++);
                encode_data_command(writeDataFile, &copyCurrentLine[startParamsIdx], 1, lineNum, encodedString, DC);
                continue;
            }

            for (i = 0; !isspace(currentLine[i]); i++) {
                operationString[i] = currentLine[i];
            }
            operationString[i] = '\0';
            operationEncode = (short) command_code(operationString);

            strcpy(copyCurrentLine, currentLine);
            copyCurrentLine[strlen(copyCurrentLine)] = '\0';

            i++; /* skip space */

            encode_regular_command(writeFile, operationEncode, &copyCurrentLine[i], lineNum, encodedString);

            if (maxLabelLength != -1) memset(labelName, 0, maxLabelLength);
            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(copyCurrentLine, 0, MAX_WORD_LENGTH);

            (*IC) += L;
            continue;
        }

        for (i = 0; isspace(currentLine[i]); i++);
        /* if a label is declared */
        for (; currentLine[i] != ':'; i++) {
            currentLabelLength++;
        }
        /* allocating memory */
        if (labelName == NULL) {
            maxLabelLength = currentLabelLength;
            labelName = (char *) malloc(maxLabelLength);
        } else if (maxLabelLength < currentLabelLength) {
            labelName = (char *) realloc(labelName, maxLabelLength);
            maxLabelLength = currentLabelLength;
        }
        /* memory allocation was unsuccessful */
        if (labelName == NULL) {
            fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
        /* initializing labelName */
        strncpy(labelName, &currentLine[i - currentLabelLength], currentLabelLength);
        labelName[currentLabelLength] = '\0';
        /* skipping colon and space */
        i += 2;
        if (labelArraySize == labelsInArray) {
            labelArraySize += DEF_ARRAY_SIZE;
            labelsArray = (char **) realloc(labelsArray, labelArraySize);
            if (labelsArray == NULL) {
                fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
                return MEMORY_NOT_ALLOCATED_ERROR_CODE;
            }
        }
        labelsArray[labelsInArray] = (char *) malloc(maxLabelLength);
        if (labelsArray[labelsInArray] == NULL) {
            fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
        strncpy(labelsArray[labelsInArray], labelName, currentLabelLength);
        labelsArray[labelsInArray][currentLabelLength] = '\0';
        labelsInArray++;


        /* .data or .string */
        if (currentLine[i] == '.') {
            /* setting storeDataString */
            for (startParamsIdx = i; !isspace(currentLine[startParamsIdx]); startParamsIdx++) {
                storeDataString[startParamsIdx - i] = currentLine[startParamsIdx];
            }
            storeDataString[startParamsIdx - i] = '\0';

            /* handling case where ".entry labelName" command was before definition of label */
            if (contains_key_int(entriesTable, labelName)) {
                change_value_int(entriesTable, labelName, *DC, 1);
            } else if (!contains_key_int(externsTable, labelName)) {
                insertLabelReturnCode = insert_int(table, labelName, *DC, 1);

                if (insertLabelReturnCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                    fprintf(stderr, "Label %s already exists!\n", labelName);
                    continueToSecondScan = 0;
                    continue;
                }
            }

            for (startParamsIdx = startParamsIdx + 1; isspace(
                    currentLine[startParamsIdx]); startParamsIdx++); /* skip spaces */

            /* .data declaration */
            if (strcmp(storeDataString, DATA_STORE_COMMAND) == 0) {
                encode_data_command(writeDataFile, &copyCurrentLine[startParamsIdx], 1, lineNum, encodedString, DC);
                /* .string declaration */
            } else if (strcmp(storeDataString, STRING_STORE_COMMAND) == 0) {
                startParamsIdx++; /* skipping the \" at the beginning */
                encode_data_command(writeDataFile, &copyCurrentLine[startParamsIdx], 0, lineNum, encodedString, DC);
            }

            if (maxLabelLength != -1) memset(labelName, 0, maxLabelLength);
            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(copyCurrentLine, 0, MAX_WORD_LENGTH);
            memset(storeDataString, 0, STRING_STORE_COMMAND_LENGTH);
            continue;
            /* not storing data */
        } else {
            if (contains_key_int(entriesTable, labelName)) {
                change_value_int(entriesTable, labelName, *IC, 0);
            } else if (!contains_key_int(entriesTable, labelName)
                       && !contains_key_int(externsTable, labelName)) {
                insertLabelReturnCode = insert_int(table, labelName, *IC, 0);

                if (insertLabelReturnCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                    fprintf(stderr, "Label %s already exists!\n", labelName);
                    continueToSecondScan = 0;
                    continue;
                }
                /* already declared */
            } else if (contains_key_int(table, labelName)) {
                fprintf(stderr, "%s: %s", labelName, REDEFINITION_OF_LABEL_ERROR_MESSAGE);
                continueToSecondScan = 0;
                continue;
            }

            for (startParamsIdx = 0; currentLine[startParamsIdx] != ':'; startParamsIdx++);
            /* skip space */
            startParamsIdx++;
            strcpy(copyCurrentLine, currentLine);
            token = strtok(&copyCurrentLine[startParamsIdx], " \t\n,");
            /* skip operation instruction name and space after it */
            startParamsIdx +=(int) strlen(token) + 2;

            operationEncode = command_code(token);
            strcpy(copyCurrentLine, currentLine);

            encode_regular_command(writeFile, operationEncode, &copyCurrentLine[startParamsIdx], lineNum,
                                   encodedString);

            if (maxLabelLength != -1) memset(labelName, 0, maxLabelLength);
            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(copyCurrentLine, 0, MAX_WORD_LENGTH);
            (*IC) += L;
            continue;
        }
    }

    /* incrementing all local symbol's table values by IC */
    for (i = 0; i < table->size; i++) {
        if (table->items[i]) {
            current = table->items[i];
            while (current) {
                if (current->isData) current->value += *IC;
                current = current->next;
            }
        }

        if (entriesTable->items[i]) {
            current = entriesTable->items[i];
            while (current) {
                if (current->isData) current->value += *IC;
                current = current->next;
            }
        }
    }
    /* checking if any entries exist */
    for (i = 0; i < entriesTable->size; i++) {
        if (entriesTable->items[i]) {
            areEntries = 1;
            break;
        }
    }

    if (areEntries) {
        /* +4 for .ent */
        newEntryFileName = (char *) calloc(fileNameLength + strlen(entryFilePostfix), sizeof(char));
        if (newEntryFileName == NULL) {
            fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
        for (i = 0; i < fileNameLength; i++) {
            /* don't want the .ob postfix */
            if (fileName[i] == '.') break;
            newEntryFileName[i] = fileName[i];
        }
        strcat(newEntryFileName, entryFilePostfix);
        newEntryFileName[i + 4] = '\0';
        if ((entriesFile = fopen(newEntryFileName, WRITE_MODE)) == NULL) {
            fprintf(stderr, "Error trying to open file %s\n", newEntryFileName);
            remove(newEntryFileName);
            free(newEntryFileName);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
        for (i = 0; i < labelsInArray; i++) {
            if (contains_key_int(entriesTable, labelsArray[i])) {
                fprintf(entriesFile, "%s\t%d\n", labelsArray[i], get_value_int(entriesTable, labelsArray[i]));
            }
        }
    }

    return continueToSecondScan;
}

int second_scan(char *fileName, FILE *readFile, FILE *writeFile, hashTableInt *table, hashTableInt *entriesTable,
                hashTableInt *externsTable, int *IC, int *DC) {
    int i;
    int L = 100 - 1;
    int areExterns = 0;
    char currentLine[MAX_WORD_LENGTH];
    char encodedString[NUM_BITS];
    int labelEncode;

    FILE *externsFile;
    int fileNameLength;
    char *newExternFileName = NULL;
    const char externFilePostfix[] = ".ext";

    fileNameLength =(int) strlen(fileName);

    /* checking if any externs exist */
    for (i = 0; i < externsTable->size; i++) {
        if (externsTable->items[i]) {
            areExterns = 1;
            break;
        }
    }

    if (areExterns) {
        /* +4 for .ent */
        newExternFileName = (char *) calloc(fileNameLength + strlen(externFilePostfix), sizeof(char));
        if (newExternFileName == NULL) {
            fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
        for (i = 0; i < fileNameLength; i++) {
            /* don't want the .ob postfix */
            if (fileName[i] == '.') break;
            newExternFileName[i] = fileName[i];
        }
        strcat(newExternFileName, externFilePostfix);
        newExternFileName[i + 4] = '\0';
        if ((externsFile = fopen(newExternFileName, WRITE_MODE)) == NULL) {
            fprintf(stderr, "Error trying to open file %s\n", newExternFileName);
            remove(newExternFileName);
            free(newExternFileName);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
    }

    /* writing IC and DC to top of file */
    fprintf(writeFile, "\t\t%d\t%d\n", (*IC - 100), *DC);
    *IC = 100;
    *DC = 0;

    while (fgets(currentLine, MAX_WORD_LENGTH, readFile) != NULL) {
        L++;
        if (currentLine[strlen(currentLine) - 1] == '\n') currentLine[strlen(currentLine) - 1] = '\0';
        labelEncode = 0;

        for (i = 0; !isspace(currentLine[i]); i++);
        i++; /* skip tab */

        /* encoded line doesn't need to be rewritten */
        if (currentLine[i] < '8' && currentLine[i] >= '0') {
            if (L < 1000) fprintf(writeFile, "%d%d\t%s\n", 0, L, &currentLine[i]);
            else fprintf(writeFile, "%d\t%s\n", L, &currentLine[i]);
            continue;
        }

        /* encode it from table/entriesTable/externsTable */
        if (is_label(&currentLine[i])) {
            /* an entry label */
            if (contains_key_int(entriesTable, &currentLine[i])) {
                labelEncode = get_value_int(entriesTable, &currentLine[i]);
                write_to_ob_file((labelEncode << SHIFTS_FOR_DEST) | R, encodedString, L, writeFile);
                /* a regular label */
            } else if (contains_key_int(table, &currentLine[i])) {
                labelEncode = get_value_int(table, &currentLine[i]);
                labelEncode = (labelEncode << SHIFTS_FOR_DEST) | R;
                write_to_ob_file(labelEncode, encodedString, L, writeFile);
                /* an extern label */
            } else if (contains_key_int(externsTable, &currentLine[i])) {
                labelEncode = 0;
                labelEncode = labelEncode | E;
                write_to_ob_file(labelEncode, encodedString, L, writeFile);
                /* writing to extern file (with word number it was shown at) */
                if (areExterns) {
                    if (L < 1000) fprintf(externsFile, "%s\t%d%d\n", &currentLine[i], 0, L);
                    else fprintf(externsFile, "%s\t%d\n", &currentLine[i], L);
                }
                /* label wasn't defined at all */
            } else {
                fprintf(stderr, "%s: %s", &currentLine[i], LABEL_UNDEFINED_ERROR_MESSAGE);
                remove(newExternFileName);
                free(newExternFileName);
                return 0;
            }
        }
    }

    /* remove temp file */
    free(newExternFileName);

    return 1;
}


int encode_regular_command(FILE *writeFile, short operationEncode, char *line, int lineNum, char encodedString[]) {
    int j;
    char *token = NULL;
    int firstRegisterIdx = -1, secondRegisterIdx = -1;
    int firstRegisterAddressIdx = -1, secondRegisterAddressIdx = -1;
    char labelBeforeParams[MAX_WORD_LENGTH];
    int isEncodedFirst = 0, isEncodedSecond = 0;
    short firstParamEncode, secondParamEncode;
    firstParamEncode = secondParamEncode = 0;

    operationEncode = (operationEncode << SHIFTS_FOR_OPCODE) | A;
    for (j = 0; j < MAX_WORD_LENGTH; j++) labelBeforeParams[j] = '\0';
    j = 0;

    switch (operationEncode >> SHIFTS_FOR_OPCODE) {
        case MOV_CODE:
        case CMP_CODE:
        case ADD_CODE:
        case SUB_CODE: {
            token = strtok(line, " \t\n,#");
            while (token != NULL) {
                if (!isEncodedFirst) {
                    if ((firstRegisterIdx = is_register(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_SRC_REGISTER);
                    } else if ((firstRegisterAddressIdx = is_register_address(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_SRC_REGISTER_ADDRESS);
                    }
                        /* in first_scan setting labels encoding as the labels names in .ob file (encode them in 2nd scan) */
                    else if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_SRC_LABEL);
                        strcpy(labelBeforeParams, token);
                    }
                        /* it's a number */
                    else {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_SRC_DIRECT);
                        firstParamEncode = (short) atoi(token);
                        firstParamEncode = ((firstParamEncode << SHIFTS_FOR_DEST) | A);
                    }
                    isEncodedFirst = 1;
                } else if (!isEncodedSecond) {
                    if ((secondRegisterIdx = is_register(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        /* need to encode both registers in one word */
                        if (firstRegisterIdx != -1) {
                            write_to_ob_file((firstRegisterIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) |
                                             (secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                            firstRegisterIdx = -1;
                        } else if (firstRegisterAddressIdx != -1) {
                            write_to_ob_file((firstRegisterAddressIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) |
                                             (secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                            firstRegisterAddressIdx = -1;
                        } else {
                            /* encode 2 separate words */
                            /* source operand was a label */
                            if (labelBeforeParams[0] != '\0') {
                                fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                                /* source operand was a number */
                            } else write_to_ob_file(firstParamEncode, encodedString, lineNum, writeFile);
                            /* encode destination operand which was a register */
                            write_to_ob_file((secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A, encodedString,
                                             lineNum, writeFile);
                        }
                        /* in first_scan setting labels' names in .temp_ob file (encode them in 2nd scan) */

                    } else if ((secondRegisterAddressIdx = is_register_address(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER_ADDRESS);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        /* need to encode both registers in one word */
                        if (firstRegisterIdx != -1) {
                            write_to_ob_file((firstRegisterIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) |
                                             (secondRegisterAddressIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                            firstRegisterIdx = -1;
                        } else if (firstRegisterAddressIdx != -1) {
                            write_to_ob_file((firstRegisterAddressIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) |
                                             (secondRegisterAddressIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                            firstRegisterAddressIdx = -1;
                        } else {
                            /* encode 2 separate words */
                            /* source operand was a label */
                            if (labelBeforeParams[0] != '\0') {
                                fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                                /* source operand was a number */
                            } else write_to_ob_file(firstParamEncode, encodedString, lineNum, writeFile);
                            /* encode destination operand which was a register */
                            write_to_ob_file((secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A, encodedString,
                                             lineNum, writeFile);
                        }
                        /* in first_scan setting labels' names in .temp_ob file (encode them in 2nd scan) */

                    } else if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_LABEL);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        /* source operand was a register */
                        if (firstRegisterIdx != -1)
                            write_to_ob_file((firstRegisterIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) | A, encodedString,
                                             lineNum, writeFile);
                            /* source operand was a label */
                        else if (firstRegisterAddressIdx != -1)
                            write_to_ob_file((firstRegisterAddressIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                        else if (labelBeforeParams[0] != '\0') {
                            fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                            /* source operand was a number */
                        } else write_to_ob_file(firstParamEncode, encodedString, lineNum, writeFile);
                        fprintf(writeFile, "%d\t%s\n", lineNum, token);
                    }
                        /* it's a number (ONLY for cmp) */
                    else if (operationEncode >> SHIFTS_FOR_OPCODE == CMP_CODE) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_DIRECT);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);

                        secondParamEncode = (short) atoi(token);
                        secondParamEncode = (secondParamEncode << SHIFTS_FOR_DEST) | A;
                        /* source operand was a register */
                        if (firstRegisterIdx != -1) {
                            write_to_ob_file((firstRegisterIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) | A, encodedString,
                                             lineNum, writeFile);
                            /* source operand was a label */
                        } else if (firstRegisterAddressIdx != -1)
                            write_to_ob_file((firstRegisterAddressIdx << SHIFTS_FOR_FIRST_PARAM_REGISTER) | A,
                                             encodedString, lineNum, writeFile);
                        else if (labelBeforeParams[0] != '\0') {
                            fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                            /* source operand was a number */
                        } else write_to_ob_file(firstParamEncode, encodedString, lineNum, writeFile);
                        write_to_ob_file(secondParamEncode, encodedString, lineNum, writeFile);
                        secondParamEncode = 0;
                    }
                    isEncodedSecond = 1;
                    firstRegisterIdx = secondRegisterIdx = -1;
                    firstRegisterAddressIdx = secondRegisterAddressIdx = -1;

                }
                token = strtok(NULL, " \t\n,#");
            }
            isEncodedFirst = isEncodedSecond = 0;
            break;
        }

        case LEA_CODE: {
            token = strtok(line, " \t\n,");
            while (token != NULL) {
                if (!isEncodedFirst) {
                    if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_SRC_LABEL);
                        strcpy(labelBeforeParams, token);
                    }
                    isEncodedFirst = 1;
                } else if (!isEncodedSecond) {
                    if ((secondRegisterIdx = is_register(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        /* source operand was a label */
                        if (labelBeforeParams[0] != '\0') {
                            fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                        }
                        /* encode destination operand which was a register */
                        write_to_ob_file((secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A, encodedString,
                                         lineNum, writeFile);
                        /* in first_scan setting labels' names in .temp_ob file (encode them in 2nd scan) */

                    } else if ((secondRegisterAddressIdx = is_register_address(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER_ADDRESS);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        /* source operand was a label */
                        if (labelBeforeParams[0] != '\0') {
                            fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                            /* source operand was a number */
                        }
                        /* encode destination operand which was a register */
                        write_to_ob_file((secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A, encodedString,
                                         lineNum, writeFile);
                        /* in first_scan setting labels' names in .temp_ob file (encode them in 2nd scan) */

                    } else if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_LABEL);
                        /* encode first word */
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        if (labelBeforeParams[0] != '\0') {
                            fprintf(writeFile, "%d\t%s\n", lineNum, labelBeforeParams);
                            /* source operand was a number */
                        }
                        fprintf(writeFile, "%d\t%s\n", lineNum, token);
                    }
                    isEncodedSecond = 1;
                    firstRegisterIdx = secondRegisterIdx = -1;
                    firstRegisterAddressIdx = secondRegisterAddressIdx = -1;
                }
                token = strtok(NULL, " \t\n,");
            }
            isEncodedFirst = isEncodedSecond = 0;
            break;
        }

            /* only destination operand instructions (no jmp, bne, jsr) can be registers or label only */
        case CLR_CODE:
        case NOT_CODE:
        case INC_CODE:
        case DEC_CODE:
        case RED_CODE:
        case PRN_CODE: {
            token = strtok(line, " \t\n#");
            while (token != NULL) {
                if (!isEncodedFirst) {
                    if ((secondRegisterIdx = is_register(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        secondRegisterIdx = (secondRegisterIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A;
                        write_to_ob_file(secondRegisterIdx, encodedString, lineNum, writeFile);
                        secondRegisterIdx = -1;
                        /* in first_scan setting labels as ? in .ob file (encode them in 2nd scan) */
                    } else if ((secondRegisterAddressIdx = is_register_address(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER_ADDRESS);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        secondRegisterAddressIdx = (secondRegisterAddressIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A;
                        write_to_ob_file(secondRegisterAddressIdx, encodedString, lineNum, writeFile);
                        secondRegisterAddressIdx = -1;
                    } else if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_LABEL);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        fprintf(writeFile, "%d\t%s\n", lineNum, token);
                    } else {
                        /* checking immediate addressing (ONLY for prn) */
                        if (operationEncode >> SHIFTS_FOR_OPCODE == PRN_CODE) {
                            operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_DIRECT);
                            write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                            /* check immediate addressing */
                            secondParamEncode = (short) atoi(token);
                            secondParamEncode = (secondParamEncode << SHIFTS_FOR_DEST) | A;
                            write_to_ob_file(secondParamEncode, encodedString, lineNum, writeFile);
                            secondParamEncode = 0;
                        }
                    }
                    isEncodedFirst = 1;
                    secondRegisterIdx = -1;
                    secondRegisterAddressIdx = -1;
                }
                token = strtok(NULL, " \t\n#");
            }
            isEncodedFirst = isEncodedSecond = 0;

            break;
        }

            /* still no source operand */
        case JMP_CODE:
        case BNE_CODE:
        case JSR_CODE: {
            token = strtok(line, " \t\n");
            while (token != NULL) {
                if (!isEncodedFirst) {
                    if ((secondRegisterAddressIdx = is_register_address(token)) != -1) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_REGISTER_ADDRESS);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        secondRegisterAddressIdx = (secondRegisterAddressIdx << SHIFTS_FOR_SECOND_PARAM_REGISTER) | A;
                        write_to_ob_file(secondRegisterAddressIdx, encodedString, lineNum, writeFile);
                        secondRegisterAddressIdx = -1;
                    } else if (is_label(token)) {
                        operationEncode = operationEncode | (BIT << SHIFTS_FOR_DEST_LABEL);
                        write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
                        fprintf(writeFile, "%d\t%s\n", lineNum, token);
                    }
                    isEncodedFirst = 1;
                    secondRegisterIdx = -1;
                    secondRegisterAddressIdx = -1;
                }
                token = strtok(NULL, " \t\n");
            }
            isEncodedFirst = isEncodedSecond = 0;
            break;
        }

        case RTS_CODE:
        case STOP_CODE: {
            write_to_ob_file(operationEncode, encodedString, lineNum, writeFile);
            break;
        }

        default:
            return 0;
    }

    return 1;
}

void encode_data_command(FILE *writeFile, char *line, int isData, int lineNum, char encodedString[], int *DC) {
    char *token;
    short encodedWord = 0;
    int j;

    /* it's a .data declaration */
    if (isData) {
        token = strtok(line, ", \n");
        while (token != NULL) {
            encodedWord = (short) atoi(token);
            write_to_ob_file(encodedWord, encodedString, lineNum, writeFile);
            (*DC)++;
            token = strtok(NULL, ", \n");
        }
        return;
    }

    /* it's a .string declaration */
    /* skipping space between .string and '"' and counting characters until '"' */
    for (j = 0; line[j] != '"'; j++) {
        encodedWord = (short) line[j];
        write_to_ob_file(encodedWord, encodedString, lineNum, writeFile);
        (*DC)++;
    }

    /* writing \0 value at the end */
    write_to_ob_file(0, encodedString, lineNum, writeFile);
    (*DC)++;
}

void convert_to_octal(int num, char finalString[]) {
    int i = 0, j = 0;
    int octal_digit;

    while (i < NUM_BITS) {
        octal_digit = 0;

        if (i < NUM_BITS && (num & (1 << (NUM_BITS - 1 - i)))) {
            octal_digit += 4;
        }
        i++;
        if (i < NUM_BITS && (num & (1 << (NUM_BITS - 1 - i)))) {
            octal_digit += 2;
        }
        i++;
        if (i < NUM_BITS && (num & (1 << (NUM_BITS - 1 - i)))) {
            octal_digit += 1;
        }
        i++;
        finalString[j++] = octal_digit + '0';
    }
    finalString[j] = '\0';
}

void write_to_ob_file(int num, char finalString[], int lineNum, FILE *file) {
    convert_to_octal(num, finalString);
    if (lineNum < 1000) fprintf(file, "%d%d\t%s\n", 0, lineNum, finalString);
    else fprintf(file, "%d\t%s\n", lineNum, finalString);
}

int command_code(char *command) {
    int i, spaceIdx;
    char *commandSubstring;
    int commandCode = UNKNOWN_CODE;

    if (strcmp(command, "stop") == 0) return STOP_CODE;

    for (spaceIdx = 0; command[spaceIdx] != ' ' && command[spaceIdx] != '\t'; spaceIdx++);
    commandSubstring = (char *) malloc(spaceIdx);

    if (commandSubstring == NULL) {
        fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
        return MEMORY_NOT_ALLOCATED_ERROR_CODE;
    }

    for (i = 0; i < spaceIdx; i++) commandSubstring[i] = command[i];

    if (strcmp(commandSubstring, "mov") == 0) commandCode = MOV_CODE;
    if (strcmp(commandSubstring, "cmp") == 0) commandCode = CMP_CODE;
    if (strcmp(commandSubstring, "add") == 0) commandCode = ADD_CODE;
    if (strcmp(commandSubstring, "sub") == 0) commandCode = SUB_CODE;
    if (strcmp(commandSubstring, "lea") == 0) commandCode = LEA_CODE;
    if (strcmp(commandSubstring, "clr") == 0) commandCode = CLR_CODE;
    if (strcmp(commandSubstring, "not") == 0) commandCode = NOT_CODE;
    if (strcmp(commandSubstring, "inc") == 0) commandCode = INC_CODE;
    if (strcmp(commandSubstring, "dec") == 0) commandCode = DEC_CODE;
    if (strcmp(commandSubstring, "jmp") == 0) commandCode = JMP_CODE;
    if (strcmp(commandSubstring, "bne") == 0) commandCode = BNE_CODE;
    if (strcmp(commandSubstring, "red") == 0) commandCode = RED_CODE;
    if (strcmp(commandSubstring, "prn") == 0) commandCode = PRN_CODE;
    if (strcmp(commandSubstring, "jsr") == 0) commandCode = JSR_CODE;
    if (strcmp(commandSubstring, "rts") == 0) commandCode = RTS_CODE;

    free(commandSubstring);
    return commandCode;
}


int is_label(char *string) {
    int i;

    if (strlen(string) == 0) return 0;
    if (!isalpha(string[0])) return 0;

    for (i = 1; string[i] != '\0'; i++) {
        if (!isalpha(string[i]) && !isdigit(string[i])) return 0;
    }
    return 1;
}

int is_register(char *string) {
    int i;
    for (i = 0; i < NUM_REGISTERS; i++) {
        /* returning the register index */
        if (strcmp(REGISTERS_NAMES[i], string) == 0) return i;
    }
    return -1;
}

int is_register_address(char *string) {
    int i;
    for (i = 0; i < NUM_REGISTERS; i++) {
        /* returning the register address index */
        if (strcmp(REGISTERS_ADDRESS_NAMES[i], string) == 0) return i;
    }
    return -1;
}

void combine_files(FILE *target, FILE *source) {
    char line[MAX_WORD_LENGTH];
    while (fgets(line, MAX_WORD_LENGTH, source) != NULL) {
        fprintf(target, "%s", line);

    }
}
