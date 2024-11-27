
#include "input_validation.h"

int firstGroupOps(int operation, char *line) {
    int i = 0;
    int wordSize = 0;
    char *firstParameter;
    char *secondParameter;
    int copyFromMem = 0;
    int numOfWords = 3; /* number of words to encode */

    /* skip whitespaces */
    while (isspace(line[i]))
        i++;

    /* get first argument */
    copyFromMem = i;
    while (line[i] != ',' && line[i] != '\0' && !isspace(line[i])) i++;
    wordSize = i - copyFromMem;
    firstParameter = (char *) malloc(sizeof(char *) * wordSize);

    if (firstParameter == NULL) {
        errors(22);
        return 0;
    }
    copyWord(&line[copyFromMem], firstParameter, wordSize);
    if (line[i] == '\0') {
        errors(19);
        free(firstParameter);
        return 0;
    }
    while(isspace(line[i])) i++;

    if (line[i] != ',') {
        errors(7);
        free(firstParameter);
        return 0;
    }

    /* get second argument */
    i++; /* skip first comma */
    while (isspace(line[i]) && line[i] != '\0') i++;
    copyFromMem = i; /* start copying the second word */

    if (line[i] == '\0') {
        errors(19);
        free(firstParameter);
        return 0;
    }
    while ((isalpha(line[i]) || isdigit(line[i])) || line[i] == '#' || line[i] == '+' || line[i] == '-' ||
           line[i] == '*')
        i++;

    wordSize = i - copyFromMem;
    secondParameter = (char *) malloc(sizeof(char *) * wordSize);

    if (secondParameter == NULL) {
        errors(22);
        return 0;
    }
    copyWord(&line[copyFromMem], secondParameter, wordSize);


    if (groupOneFirstArg(firstParameter, operation) == 0) {
        free(firstParameter);
        free(secondParameter);
        return 0;
    }


    if (groupOneSecondArg(secondParameter, operation) == 0) {
        free(firstParameter);
        free(secondParameter);
        return 0;
    }

    if (!terminatedCorrectly(line, i)) {
        errors(4);
        free(firstParameter);
        free(secondParameter);
        return 0;
    }

    if ((isRegister(firstParameter) && isRegister(secondParameter)) ||
        (isRegister(firstParameter) && isRegisterAddress(secondParameter)) ||
        (isRegisterAddress(firstParameter) && isRegister(secondParameter)) ||
        (isRegisterAddress(firstParameter) && isRegisterAddress(secondParameter))) {
        numOfWords = 2;
    }
    free(firstParameter);
    free(secondParameter);
    return numOfWords;
}

int secondGroupOps(char *line, int operation) {
    int i = 0;
    int wordSize = 0;
    int copyFromMem = 0;
    int numOfWords = 2;
    char *argument;

    /* skip whitespaces */
    while (isspace(line[i]) && line[i] != '\0') i++;
    /* get first argument */
    copyFromMem = i;
    while ((isalpha(line[i]) || isdigit(line[i])) || line[i] == '#' || line[i] == '+' || line[i] == '-' ||
           line[i] == '*')
        i++;
    wordSize = i - copyFromMem;
    argument = (char *) malloc(sizeof(char *) * wordSize);

    if (argument == NULL) {
        errors(22);
        return 0;
    }
    copyWord(&line[copyFromMem], argument, wordSize);
    switch (operation) {
        case CLR_CODE:
        case NOT_CODE:
        case INC_CODE:
        case DEC_CODE:
        case RED_CODE:
            if (!validLabel(argument) && !validRegister(argument) && !validRegisterAddress(argument)) {
                free(argument);
                return 0;
            }
            break;
        case PRN_CODE:
            if (!immediateAddressing(argument) && !validLabel(argument) && !validRegister(argument) &&
                !validRegisterAddress(argument)) {
                free(argument);
                return 0;
            }
            break;
        case JMP_CODE:
        case BNE_CODE:
        case JSR_CODE:
            if (!validLabel(argument) && !validRegisterAddress(argument)) {
                free(argument);
                return 0;
            }
            while (isspace(line[i]) && line[i] != '\0') i++;
            if (!terminatedCorrectly(line, i)) {
                errors(4);
                return 0;
            }
    }
    free(argument);
    return numOfWords;
}

int groupOneFirstArg(char *word, int operation) {
    char firstCharacter = word[0];

    if (operation >= MOV_CODE && operation <= SUB_CODE) {
        if (firstCharacter == '#') {
            if (immediateAddressing(word) == 0) {
                return 0;
            }
        } else if (!isLabel(word, 0) && !isRegister(word) && !isRegisterAddress(word)) {
            /* not a label, not a register and not a register address*/
            return 0;
        }
    } else if (operation == LEA_CODE && immediateAddressing(word)) {
        return 0;
    }
    return 1;
}

/* send the word into this function with one comma already checked in order to check multiple commas */
int groupOneSecondArg(char *word, int operation) {
    char firstCharacter = word[0];

    if (firstCharacter == ',') {
        return 0;
    } else if (strlen(word) == 0) {
        return 0;
    } else if (operation == CMP_CODE && firstCharacter == '#') {
        if (!(immediateAddressing(word))) {
            return 0;
        } else
            return 1;
    } else if (!isRegister(word) && !isLabel(word, 0) && !isRegisterAddress(word)) {
        return 0;
    }

    return 1;
}

int validRegister(char *line) {
    if (strlen(line) == 0) {
        /* missing parameter */
        errors(19);
        return 0;
    } else if (!isRegister(line)) {
        return 0;
    }
    return 1;
}

int validLabel(char *line) {
    if (strlen(line) == 0) {
        /* missing parameter */
        errors(19);
        return 0;
    } else if (!isLabel(line, 0)) {
        return 0;
    }
    return 1;
}

int validRegisterAddress(char *line) {
    if (strlen(line) == 0) {
        /* missing parameter */
        errors(19);
        return 0;
    } else if (!isRegisterAddress(line)) {
        return 0;
    }
    return 1;
}

int thirdGroupOps(char *line) {
    int i = 0;
    while (isalpha(line[i])) i++;
    return terminatedCorrectly(line, i);
}

int validData(char *line) {
    int i = 0;
    enum status state;
    char *currentNum;
    int copyFromHere;
    int numOfDigits;
    int numOfWords = 0;
    state = NUMBER;
    /* skip whitespaces */
    while (isspace(line[i])) i++;

    while (line[i] != '\0') {
        switch (state) {
            case NUMBER:
                numOfWords += 1;
                copyFromHere = i;
                while (!isspace(line[i]) && line[i] != ',' && line[i] != '\0') i++;
                numOfDigits = i - copyFromHere;
                currentNum = (char *) malloc(sizeof(char *) * numOfDigits);
                if (currentNum == NULL) {
                    errors(22);
                    return 0;
                }
                copyWord(&line[copyFromHere], currentNum, numOfDigits);
                /* invalid number input */
                if (validNumber(currentNum) == 0) {
                    free(currentNum);
                    return 0;
                }

                /* continue running until ',' or '\0' */
                while (isspace(line[i])) i++;
                if (line[i] != ',' && line[i] != '\0') {
                    free(currentNum);
                    return 0;
                }

                if (line[i] == '\0')
                    state = DONE;
                else
                    state = COMMA;
                break;

            case COMMA:
                i++;
                while (isspace(line[i])) i++;
                if (line[i] != '+' && line[i] != '-' && !isdigit(line[i])) {
                    errors(10);
                    free(currentNum);
                    return 0;
                }
                state = NUMBER;
                break;

            case DONE:
                free(currentNum);
                return 1;
        }
    }
    free(currentNum);
    return numOfWords;
}

int validString(char *line) {
    int i = 0;
    int finalChar = strlen(line) - 1;
    /*skip whitespaces */
    while (isspace(line[i])) i++;

    if (line[i] != '"' || line[finalChar] != '"')
        return 0;
    return strlen(&line[i]) - 2;
}

int validEntryOrExtern(char *line) {
    int i = 0;
    int copyFrom = 0;

    /*skip whitespaces */
    while (isspace(line[i])) i++;
    copyFrom = i;

    if (line[i] == '\0') {
        errors(19);
        return 0;
    }

    if (isRegister(&line[i])) {
        errors(21);
        return 0;
    }

    if (isRegisterAddress(&line[i])) {
        errors(21);
        return 0;
    }

    while (isalpha(line[i]) || isdigit(line[i])) i++;
    while (isspace(line[i])) i++;

    if (line[i] != '\0') {
        errors(4);
        return 0;
    }
    return isLabel(&line[copyFrom], 0);
}
