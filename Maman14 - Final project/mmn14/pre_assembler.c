
#include "pre_assembler.h"
#include "lexer.h"

int deploy_macros(FILE *readFile, FILE *writeFile, hashTable *labelTable, int *longestMacroBodyLength) {
    calculate_longest_macro_body(readFile, longestMacroBodyLength);
    /* going back to start of readFile to reiterate it for writing */
    rewind(readFile);
    if (!read_macros_from_file(readFile, labelTable, longestMacroBodyLength))
        return 0;
    /* going back to start of readFile to reiterate it for writing */
    rewind(readFile);
    write_macros_to_file(readFile, writeFile, labelTable);
    *longestMacroBodyLength = -1;

    /* freeing files and hash table */
    fclose(readFile);
    fclose(writeFile);

    free(labelTable);
    return 1;
}

int read_macros_from_file(FILE* file, hashTable *table, int *longestMacroBody) {
    int i, j, start, end;
    int maxMacroBodyLength = *longestMacroBody, endMacro = 1;
    int macroNameMaxLength = -1, currentMacroLength = 0;
    int tokenLength, currentLineLength = -1, insertResultCode;
    int labelsInArray = 0;
    int allocatedSize = DEF_ARRAY_SIZE;
    char *token;
    char *macroName = NULL;
    char *macroBody = NULL;
    enum macroState macroStatus;
    char currentLine[MAX_WORD_LENGTH];
    char tempLine[MAX_WORD_LENGTH];
    char cutCurrentLine[MAX_WORD_LENGTH];
    char **labelNamesArray;
    labelNamesArray = (char **)malloc(allocatedSize * sizeof(char *));
    macroStatus = NOT_IN_MACRO;

    if (maxMacroBodyLength > 0) {
        macroBody = (char *) calloc(maxMacroBodyLength, sizeof(char));
        if (macroBody == NULL) {
            fprintf(stderr, MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE);
            return MEMORY_NOT_ALLOCATED_ERROR_CODE;
        }
    }

    /* resetting macroBody */
    for (i = 0; i < maxMacroBodyLength; i++) macroBody[i] = '\0';

    while (fgets(currentLine, MAX_WORD_LENGTH, file) != NULL) {
        currentLineLength = strlen(currentLine);
        start = 0, end = currentLineLength-1;
        for (; isspace(currentLine[start]); start++);
        for (; isspace(currentLine[end]); end--);
        if (start >= end) continue;
        for (i = start; i <= end; i++) cutCurrentLine[i-start] = currentLine[i];
        i=i-start;
        if(containLabel(labelNamesArray,cutCurrentLine,labelsInArray,i)){
            labelsInArray ++;
            if(allocatedSize <= labelsInArray) {
                allocatedSize += DEF_ARRAY_SIZE;
                labelNamesArray = (char **) realloc(labelNamesArray, (allocatedSize) * sizeof(char *));
            }
        }

        if (strcmp(currentLine, "\n") == 0) {
            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(tempLine, 0, MAX_WORD_LENGTH);
            memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
            continue;
        }

        if (macroStatus == NOT_IN_MACRO) {
            strcpy(tempLine, currentLine);
            token = strtok(tempLine, " \t");

            while (token != NULL) {
                tokenLength = strlen(token);
                if (strcmp(token, MACRO_KEYWORD) == 0) {
                    macroStatus = IN_MACRO_NAME;
                    token = strtok(NULL, " \t");
                    continue;
                }
                /* reading macro name and changing to macro body state */
                if (macroStatus == IN_MACRO_NAME) {
                    if (macroNameMaxLength < tokenLength) {
                        macroNameMaxLength = tokenLength;
                        if (macroName == NULL) macroName = (char *) malloc(macroNameMaxLength);
                        else macroName = (char *) realloc(macroName, macroNameMaxLength);
                    }
                    /* setting macroName */
                    strcpy(macroName, token);
                    macroStatus = IN_MACRO_BODY;

                }
                token = strtok(NULL, " \t");
                if (macroStatus == IN_MACRO_BODY) {
                    /* Check if there are extra characters after the macro name */
                    if (token != NULL) {
                        fprintf(stderr, "%s: %s", macroName, ILLEGAL_TEXT_AFTER_MACRO_NAME_ERROR_MESSAGE);
                        free(macroName); 
                        for ( i = 0; i < labelsInArray ; i++) {
                            free(labelNamesArray[i]);
                        }
                        free(labelNamesArray);

                        return 0;  /* Indicate an error */
                    }
                }

            }

            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(tempLine, 0, MAX_WORD_LENGTH);
            memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
            continue;
        }
        /* reading macro body */
        if (macroStatus == IN_MACRO_BODY) {

            if (strlen(cutCurrentLine) >= strlen(END_MACRO_KEYWORD)){
                for (j = 0; j < strlen(END_MACRO_KEYWORD); j++) {
                    if (cutCurrentLine[j] != END_MACRO_KEYWORD[j])
                        endMacro = 0;
                }
            }
            else
                endMacro = 0;
            if (endMacro) {
                if (strlen(cutCurrentLine) > strlen(END_MACRO_KEYWORD)){
                    macroName[strlen(macroName)-1] = '\0';
                    fprintf(stderr, "%s: %s", macroName,ILLEGAL_TEXT_AFTER_MACRO_END_ERROR_MESSAGE);
                    free(macroBody);
                    free(macroName);  /* Free the allocated memory for macroName */
                    for ( i = 0; i < labelsInArray; i++) {
                        free(labelNamesArray[i]);
                    }
                    free(labelNamesArray);
                    return 0;  /* Indicate an error */
                }

                macroStatus = NOT_IN_MACRO;
                currentMacroLength = 0;
                macroName[strlen(macroName)-1] = '\0';

                /* check if macro name is an instruction name */
                if (is_command(macroName)) {
                    /* TODO: print error macro name is an instruction */
                    fprintf(stderr, "%s: %s", macroName, MACRO_NAME_ILLEGAL_ERROR_MESSAGE);
                    for ( i = 0; i < labelsInArray; i++) {
                        free(labelNamesArray[i]);
                    }
                    free(labelNamesArray);
                    return 0;
                }

                macroBody[strlen(macroBody)] = '\0';
                insertResultCode = insert(table, macroName, macroBody);
                /* error handling */
                if (insertResultCode == HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE) {
                    fprintf(stderr, "Macro %s:\n%s",
                            macroName, MACRO_ALREADY_EXISTS_ERROR_MESSAGE);
                    free(macroName);
                    for ( i = 0; i < labelsInArray; i++) {
                        free(labelNamesArray[i]);
                    }
                    free(labelNamesArray);
                    return 0;
                } /* TODO: add more errors */

                memset(macroName, 0, macroNameMaxLength);
                memset(macroBody, 0, maxMacroBodyLength);
                memset(currentLine, 0, MAX_WORD_LENGTH);
                memset(tempLine, 0, MAX_WORD_LENGTH);
                memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
                endMacro = 1;
                continue;
            }

            cutCurrentLine[i] = '\n';
            strcat(macroBody, cutCurrentLine);
            macroBody[strlen(macroBody)] = '\0';
            currentMacroLength += strlen(cutCurrentLine);
        }

        memset(currentLine, 0, MAX_WORD_LENGTH);
        memset(tempLine, 0, MAX_WORD_LENGTH);
        memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
        endMacro = 1;
    }
    free(macroName);
    free(macroBody);
    for(i = 0 ; i < labelsInArray; i++) {
        if (contains_key(table, labelNamesArray[i])) {
            fprintf(stderr, "%s: %s", labelNamesArray[i], SAME_MACRO_LABEL_NAME_ERROR_MESSAGE);
            for (i = 0; i < labelsInArray; i++) {
                free(labelNamesArray[i]);
            }
            free(labelNamesArray);

            return 0;
        }
    }
    for ( i = 0; i < labelsInArray; i++) {
        free(labelNamesArray[i]);
    }
    free(labelNamesArray);
    return 1;
}

int write_macros_to_file(FILE *readFile, FILE *writeFile, hashTable *table) {
    int j, k, cutIdx;
    char cutWord[MAX_WORD_LENGTH];
    char shortenedCutWord[MAX_WORD_LENGTH];
    char word[MAX_WORD_LENGTH];
    char tempWord[MAX_WORD_LENGTH];
    char *token;
    int firstInLine = 1, deployedMacro = 0, enteredMacro = 0;

    enum macroState macroStatus;

    macroStatus = NOT_IN_MACRO;

    while (fgets(word, MAX_WORD_LENGTH, readFile) != NULL) {
        j = 0;
        k = strlen(word);
        while (isspace(word[j])) j++;
        if (j == k) continue;
        /* assuming comments are at beginning of line */
        if (word[j] == ';') continue;

        while (isspace(word[k])) k--;

        for (cutIdx = j; cutIdx < k+1; cutIdx++) cutWord[cutIdx-j] = word[cutIdx];

        /* encountered a comment */
        if (strstr(cutWord, ";")) {
            /* reset word and cutWord */
            memset(cutWord, 0, MAX_WORD_LENGTH);
            memset(word, 0, MAX_WORD_LENGTH);
            memset(shortenedCutWord, 0, MAX_WORD_LENGTH);
            memset(tempWord, 0, MAX_WORD_LENGTH);
            continue;
        }

        /* if finished with macro deployment OR haven't found one yet */
        if (macroStatus == NOT_IN_MACRO) {
            /* getting rid of \n character */
            if (cutWord[strlen(cutWord)-1] == '\n') cutWord[strlen(cutWord)-1] = '\0';
            strcpy(tempWord, cutWord);

            token = strtok(tempWord, " \t");
            /* traversing every word in sentence to check for macro deployments */
            while (token != NULL) {
                /* if reached a macro declaration */
                if (strcmp(token, MACRO_KEYWORD) == 0) {
                    macroStatus = IN_MACRO_BODY;
                    enteredMacro = 1;
                    break;
                }
                /* checking if macro is supposed to be deployed here */
                if (contains_key(table, token)) {
                    if (!firstInLine) fprintf(writeFile, "%s", " ");
                    fprintf(writeFile, "%s", get_value(table, token));
                    deployedMacro = 1;
                } else {
                    if (firstInLine) {
                        fprintf(writeFile, "%s", token);
                        firstInLine = 0;
                    } else {
                        fprintf(writeFile, " %s", token);
                    }
                }

                token = strtok(NULL, " \t");
            }

            if (!deployedMacro && !enteredMacro) {
                fprintf(writeFile, "%s", "\n");
                enteredMacro = 0;
            } else deployedMacro = 0;

            firstInLine = 1;

            /* reset word and cutWord */
            memset(cutWord, 0, MAX_WORD_LENGTH);
            memset(word, 0, MAX_WORD_LENGTH);
            memset(shortenedCutWord, 0, MAX_WORD_LENGTH);
            memset(tempWord, 0, MAX_WORD_LENGTH);
            continue;
        }

        /* found a macro declaration previously, should ignore it's body */
        if (macroStatus == IN_MACRO_BODY) {
            /* if reached end of macro declaration, call it */
            if (strstr(cutWord, END_MACRO_KEYWORD) != NULL) {
                memcpy(shortenedCutWord, cutWord, strlen(cutWord)-1);
                if (strcmp(shortenedCutWord, END_MACRO_KEYWORD) == 0) {
                    /* "got out" of macro body */
                    macroStatus = NOT_IN_MACRO;
                    enteredMacro = 0;
                }
            }
        }

        /* reset word and cutWord */
        memset(cutWord, 0, MAX_WORD_LENGTH);
        memset(word, 0, MAX_WORD_LENGTH);
        memset(shortenedCutWord, 0, MAX_WORD_LENGTH);
        memset(tempWord, 0, MAX_WORD_LENGTH);
    }

    return 1;
}

void calculate_longest_macro_body(FILE *file, int *longest) {
    int i, j, start, end;
    int currentMacroBodyLength = 0, endMacro = 1;
    int currentLineLength;
    char *token;
    enum macroState macroStatus;
    char currentLine[MAX_WORD_LENGTH];
    char tempLine[MAX_WORD_LENGTH];
    char cutCurrentLine[MAX_WORD_LENGTH];

    macroStatus = NOT_IN_MACRO;

    while (fgets(currentLine, MAX_WORD_LENGTH, file) != NULL) {
        currentLineLength = strlen(currentLine);
        start = 0, end = currentLineLength-1;
        for (; isspace(currentLine[start]); start++);
        for (; isspace(currentLine[end]); end--);
        if (start >= end) continue;
        for (i = start; i <= end; i++) cutCurrentLine[i-start] = currentLine[i];

        if (strcmp(currentLine, "\n") == 0) continue;

        if (macroStatus == NOT_IN_MACRO) {
            strcpy(tempLine, currentLine);
            token = strtok(tempLine, " \t");

            while (token != NULL) {
                if (strcmp(token, MACRO_KEYWORD) == 0) {
                    macroStatus = IN_MACRO_NAME;
                    token = strtok(NULL, " \t");
                    continue;
                }
                /* reading macro name and changing to macro body state */
                if (macroStatus == IN_MACRO_NAME) {
                    macroStatus = IN_MACRO_BODY;
                }
                token = strtok(NULL, " \t");
            }

            memset(currentLine, 0, MAX_WORD_LENGTH);
            memset(tempLine, 0, MAX_WORD_LENGTH);
            memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
            continue;
        }

        /* reading macro body */
        if (macroStatus == IN_MACRO_BODY) {
            if (strlen(cutCurrentLine) != strlen(END_MACRO_KEYWORD)) endMacro = 0;
            else {
                for (j = 0; j < end-start+1; j++) {
                    if (cutCurrentLine[j] != END_MACRO_KEYWORD[j]) endMacro = 0;
                }
            }

            if (endMacro) {
                macroStatus = NOT_IN_MACRO;
                memset(currentLine, 0, MAX_WORD_LENGTH);
                memset(tempLine, 0, MAX_WORD_LENGTH);
                memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
                currentMacroBodyLength = 0;
                endMacro = 1;
                continue;
            }

            currentMacroBodyLength += currentLineLength;

            if (*longest < currentMacroBodyLength) {
                *longest = currentMacroBodyLength;
            }
        }

        memset(currentLine, 0, MAX_WORD_LENGTH);
        memset(tempLine, 0, MAX_WORD_LENGTH);
        memset(cutCurrentLine, 0, MAX_WORD_LENGTH);
        endMacro = 1;
    }
}

int is_command(char *line) {
    int commandCode = -1;

    if (strcmp(line, "mov") == 0) commandCode = MOV_CODE;
    else if (strcmp(line, "cmp") == 0) commandCode = CMP_CODE;
    else if (strcmp(line, "add") == 0) commandCode = ADD_CODE;
    else if (strcmp(line, "sub") == 0) commandCode = SUB_CODE;
    else if (strcmp(line, "lea") == 0) commandCode = LEA_CODE;
    else if (strcmp(line, "clr") == 0) commandCode = CLR_CODE;
    else if (strcmp(line, "not") == 0) commandCode = NOT_CODE;
    else if (strcmp(line, "inc") == 0) commandCode = INC_CODE;
    else if (strcmp(line, "dec") == 0) commandCode = DEC_CODE;
    else if (strcmp(line, "jmp") == 0) commandCode = JMP_CODE;
    else if (strcmp(line, "bne") == 0) commandCode = BNE_CODE;
    else if (strcmp(line, "red") == 0) commandCode = RED_CODE;
    else if (strcmp(line, "prn") == 0) commandCode = PRN_CODE;
    else if (strcmp(line, "jsr") == 0) commandCode = JSR_CODE;
    else if (strcmp(line, "rts") == 0) commandCode = RTS_CODE;

    /* MOV_CODE = 0, so if it was "mov" it' return false so adding 1 and initializing with -1 in case no condition is met */
    return commandCode+1;
}
int containLabel(char **labelNamesArray,char *cutCurrentLine,int labelNamesArraySize,int endOfWord){
    int i = 0;
    int isExtern = 1;
    int wordSize;
    char *temp;
    if(cutCurrentLine[i] == '.') {
        for (i = 0; i < sizeof(EXTERN_COMMAND)-1; i++) {
            if (cutCurrentLine[i] != EXTERN_COMMAND[i]) {
                isExtern = 0;
                break;
            }
        }
        if (isExtern) {
            while (isspace(cutCurrentLine[i])) i++;
            wordSize = endOfWord - i;
            temp = (char *) malloc(sizeof(char *) * wordSize + 1);
            if (temp == NULL) {
                errors(22);
                return 0;
            }
            copyWord(&cutCurrentLine[i], temp, wordSize + 1);
            temp[wordSize + 1] = '\0';
            labelNamesArray[labelNamesArraySize] = temp;
            return 1;
        }
    }
    else {
        while ((isalpha(cutCurrentLine[i]) || isdigit(cutCurrentLine[i]))) i++;
        if (cutCurrentLine[i] == ':') {
            temp = (char *) malloc(sizeof(char *) * i);
            if (temp == NULL) {
                errors(22);
                return 0;
            }
            copyWord(&cutCurrentLine[0], temp, i);
            temp[i] = '\0';
            labelNamesArray[labelNamesArraySize] = temp;
            return 1;
        }
    }
    return 0;

}

