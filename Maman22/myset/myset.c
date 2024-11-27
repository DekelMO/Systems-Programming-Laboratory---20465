#include <stdio.h>
#include "analyze.h"
#define SETS_AMOUNT 6

int main() {
    /* Variable declaration */
    Command command;
    set SETA, SETB, SETC, SETD, SETE, SETF;
    set *INPUT_SET1, *INPUT_SET2, *INPUT_SET3;
    int run = 1;
    int set_index1, set_index2, set_index3;
    set *set_array[SETS_AMOUNT];
    int i;
    char *string_array;
    char *inputLine;
    char * func;


    /* Assigning sets to the set array */
    set_array[0] = &SETA;
    set_array[1] = &SETB;
    set_array[2] = &SETC;
    set_array[3] = &SETD;
    set_array[4] = &SETE;
    set_array[5] = &SETF;

    /* Initializing sets */
   
    for (i = 0; i < SETS_AMOUNT; i++) {
        empty_set(set_array[i]);
    }
    
    /* Printing instructions */
    print_instructions();

    /* Main loop */
    while (run) {
        printf(">>>>>");
        string_array = readString();
        inputLine = string_array;
        printf("%s\n", inputLine);
        func = get_parameter (&inputLine,' ', '\t');
        
        /* Check if last character is a comma */
        if(func[strlen(func)-1] == ',') {
            printf("ERROR: Illegal comma\n");
            continue;
        }

        /* Get command */
        command = get_command(func);

        switch (command) {
            case READ:
                /* Read command */
                remove_whitespace(&inputLine);
                set_index1 = get_set(set_array, &inputLine, ',');
                if (set_index1 >= 0 && set_index1 < 6) {
                    INPUT_SET1 = set_array[set_index1];
                    if (correctly_terminated(inputLine)) {
                        if (consecutive_commas(inputLine)) {
                            int numElements = number_of_elements(inputLine);
                            char **tokens = splitString(inputLine, numElements);
                            if (tokens == NULL) {
                                printf("ERROR: Failed to split string.\n");
                                free(string_array);
                                return 1;
                            }
                            if (valid_set_members(tokens, numElements)) {
                                int *intArray = (int *) malloc(numElements * sizeof(int));
                                if (intArray == NULL) {
                                    printf("ERROR: Memory allocation failed\n");
                                    return 1;
                                }
                                convertStringsToInts(tokens, intArray, numElements);
                                if (values_in_range(intArray, numElements)) {
                                    qsort(intArray, numElements - 1, sizeof(int), compare);
                                    removeDuplicates(intArray, numElements);
                                    read_set(INPUT_SET1, intArray);
                                } else {
                                    printf("ERROR: Invalid set member - value out of range\n");
                                }
                                free(intArray);
                            } else {
                                printf("ERROR: Invalid set member - not an integer\n");
                            }
                            free(tokens);
                        } else {
                            printf("ERROR: Multiple consecutive commas\n");
                        }
                    } else {
                        printf("ERROR: List of set members is not terminated correctly\n");
                    }
                } else {
                    if(set_index1==-2){
                        printf("ERROR: Missing comma\n");
                    }
                    else
                        printf("ERROR: Undefined set name.\n");
                }
                break;

            case PRINT:
                /* Print command */
                set_index1 = get_set(set_array, &inputLine, '\0');
                if (set_index1 != -2) {
                    if (set_index1 >= 0 && set_index1 < 6) {
                        INPUT_SET1 = set_array[set_index1];
                        remove_whitespace(&inputLine);
                        print_set(INPUT_SET1);
                    } else {
                        printf("ERROR: Undefined set name.\n");
                    }
                } else {
                    printf("ERROR: Extraneous text after end of command\n");
                }
                break;

            case UNION:
                /* Union command */
                remove_whitespace(&inputLine);
                if (consecutive_commas(inputLine)) {
                    set_index1 = get_set(set_array, &inputLine, ',');
                    set_index2 = get_set(set_array, &inputLine, ',');
                    set_index3 = get_set(set_array, &inputLine, '\0');
                    if(set_index1 != -2 && set_index2 != -2){
                        if (set_index3 != -2) {
                            if ((set_index1 >= 0 && set_index1 < SETS_AMOUNT) && 
                                (set_index2 >= 0 && set_index2 < SETS_AMOUNT) && 
                                (set_index3 >= 0 && set_index3 < SETS_AMOUNT)) {
                                INPUT_SET1 = set_array[set_index1];
                                INPUT_SET2 = set_array[set_index2];
                                INPUT_SET3 = set_array[set_index3];
                                union_set(INPUT_SET1, INPUT_SET2, INPUT_SET3);
                            } else {
                                if(set_index1 == -3 || set_index2 == -3 ||set_index3 == -3)
                                    printf("ERROR: Missing parameter.\n");
                                else
                                    printf("ERROR: Undefined set name.\n");
                            }
                        } else {
                            printf("ERROR: Extraneous text after end of command\n");
                        }
                    } else {
                        printf("ERROR: Missing comma\n");
                    }
                } else {
                    printf("ERROR: Multiple consecutive commas\n");
                }
                break;

           case INTERSECT:
                /* Intersect command */
                remove_whitespace(&inputLine);
                if (consecutive_commas(inputLine)) {
                    set_index1 = get_set(set_array, &inputLine, ',');
                    set_index2 = get_set(set_array, &inputLine, ',');
                    set_index3 = get_set(set_array, &inputLine, '\0');
                    if(set_index1 != -2 && set_index2 != -2){
                        if (set_index3 != -2) {
                            if ((set_index1 >= 0 && set_index1 < SETS_AMOUNT) && (set_index2 >= 0 && set_index2 < SETS_AMOUNT) && (set_index3 >= 0 && set_index3 < SETS_AMOUNT)) {
                                INPUT_SET1 = set_array[set_index1];
                                INPUT_SET2 = set_array[set_index2];
                                INPUT_SET3 = set_array[set_index3];
                                intersect_set(INPUT_SET1, INPUT_SET2, INPUT_SET3);
                            } else {
                                if(set_index1 == -3 || set_index2 == -3 ||set_index3 == -3)
                                    printf("ERROR: Missing prameter.\n");
                                else
                                    printf("ERROR: Undefined set name.\n");
                            }
                        } else {
                            printf("ERROR: Extraneous text after end of command\n");
                        }}
                    else{
                        printf("ERROR: Missing comma\n");
                    }
                }
                else{
                    printf("ERROR: Multiple consecutive commas\n");
                }
                break;

            case SUB:
                /* Sub command */
                remove_whitespace(&inputLine);
                if (consecutive_commas(inputLine)) {
                    set_index1 = get_set(set_array, &inputLine, ',');
                    set_index2 = get_set(set_array, &inputLine, ',');
                    set_index3 = get_set(set_array, &inputLine, '\0');
                    if(set_index1 != -2 && set_index2 != -2){
                        if (set_index3 != -2) {
                            if ((set_index1 >= 0 && set_index1 < SETS_AMOUNT) && (set_index2 >= 0 && set_index2 < SETS_AMOUNT) && (set_index3 >= 0 && set_index3 < SETS_AMOUNT)) {
                                INPUT_SET1 = set_array[set_index1];
                                INPUT_SET2 = set_array[set_index2];
                                INPUT_SET3 = set_array[set_index3];
                                sub_set(INPUT_SET1, INPUT_SET2, INPUT_SET3);
                            } else {
                                if(set_index1 == -3 || set_index2 == -3 ||set_index3 == -3)
                                    printf("ERROR: Missing prameter.\n");
                                else
                                    printf("ERROR: Undefined set name.\n");
                            }
                        } else {
                            printf("ERROR: Extraneous text after end of command\n");
                        }}
                    else{
                        printf("ERROR: Missing comma\n");
                    }
                }
                else{
                    printf("ERROR: Multiple consecutive commas\n");
                }
                break;

            case SYMDIFF:
                /* Symmetric difference command */
                remove_whitespace(&inputLine);
                if (consecutive_commas(inputLine)) {
                    set_index1 = get_set(set_array, &inputLine, ',');
                    set_index2 = get_set(set_array, &inputLine, ',');
                    set_index3 = get_set(set_array, &inputLine, '\0');
                    if(set_index1 != -2 && set_index2 != -2){
                        if (set_index3 != -2) {
                            if ((set_index1 >= 0 && set_index1 < SETS_AMOUNT) && (set_index2 >= 0 && set_index2 < SETS_AMOUNT) && (set_index3 >= 0 && set_index3 < SETS_AMOUNT)) {
                                INPUT_SET1 = set_array[set_index1];
                                INPUT_SET2 = set_array[set_index2];
                                INPUT_SET3 = set_array[set_index3];
                                symdiff_set(INPUT_SET1, INPUT_SET2, INPUT_SET3);
                            } else {
                                if(set_index1 == -3 || set_index2 == -3 ||set_index3 == -3)
                                    printf("ERROR: Missing prameter.\n");
                                else
                                    printf("ERROR: Undefined set name.\n");
                            }
                        } else {
                            printf("ERROR: Extraneous text after end of command\n");
                        }}
                    else{
                        printf("ERROR: Missing comma\n");
                    }
                }
                else{
                    printf("ERROR: Multiple consecutive commas\n");
                }
                break;

            case STOP:
                run = 0;
                continue;

            case EMPTY_LINE:
                break;

            case ERROR:
                printf("ERROR: Undefined command name\n");
                break;
        }

        free(string_array);
    }
    return 0;
}
