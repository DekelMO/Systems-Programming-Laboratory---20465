#include "analyze.h"

/* Function to print program instructions */
void print_instructions() {
    printf("Welcome to the Set Manipulation Program!\n\n");

    /* Instructions for using the program*/
    printf("To use this program for reading or printing a set, enter the following format:\n");
    printf("\"Command Name\" \"Set Name\", \"Set\"\n");
    printf("Example: read_set SETA, 1, 2, 3, -1\n\n");

    printf("To use a set operation command, enter the following format:\n");
    printf("\"Command Name\" \"FIRST_SET\", \"SECOND_SET\", \"THIRD_SET\"\n");
    printf("Example: union_set SETA, SETB, SETC\n\n");

    /* Valid command names*/
    printf("Valid Command Names:\n");
    printf("\"read_set\"    \"print_set\"   \"union_set\"\n");
    printf("\"intersect_set\"    \"sub_set\"   \"symdiff_set\"\n");
    printf("\"stop\"\n\n");

    /* Valid set names */
    printf("Valid Set Names:\n");
    printf("\"SETA\"    \"SETB\"    \"SETC\"\n");
    printf("\"SETD\"    \"SETE\"    \"SETF\"\n\n");

    /* Guidelines for input  */
    printf("Guidelines:\n");
    printf("- Set values must be in the range [0, 127].\n");
    printf("- Separate each element of the set with a comma.\n");
    printf("- Separate the command from the first set input with spaces or tabs.\n");
    printf("- When using \"read_set\", end the set with \"-1\" to indicate the end of input.\n\n");

    /* Exiting the program  */
    printf("To exit the program, enter:\n");
    printf("\"stop\"\n\n");

    /* Ready indication  */
    printf("Whenever you see \">>>>\" on the screen, the program is ready for the next action.\n");

    printf("Let's begin!\n\n");
}

/* Function to read a string from standard input */
char* readString() {
    int ch; /* Temporary variable to store the character */
    char *buffer;
    int size = 50; /* Initial buffer size  */
    int numCharsRead = 0; /* Tracks the total number of characters read */

    /* Allocate memory for the buffer */
    buffer = (char*)malloc(size * sizeof(char));

    /* Read characters from standard input using a loop */
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Check if buffer needs reallocation */
        if (numCharsRead + 1 > size) {
            size *= 2; /* Double the buffer size */
            buffer = (char*)realloc(buffer, size * sizeof(char));
            if (buffer == NULL) {
                printf("Error allocating memory\n");
                return NULL; /* Indicate error*/
            }
        }

        /* Store the character in the buffer */
        buffer[numCharsRead] = ch;
        numCharsRead++;
    }

    /* Handle newline character (if present) */
    if (ch == '\n') {
        buffer[numCharsRead] = '\0'; /* Replace newline with null terminator */
    } else {
        /* Add null terminator manually if EOF is reached */
        buffer[numCharsRead] = '\0';
    }

    /* Return the read string (remember to avoid returning local variables directly)*/
    return buffer;
}

/* Function to remove leading whitespace from a string */
void remove_leading_whitespace(char **str) {
    /* Find the first non-whitespace character */
     char *temp = *str;
     if(*str == NULL)
        return;
    while (isspace(*temp)) {
        temp ++;
    }
    *str = temp;
}

/* Function to get a parameter from a string */
char *get_parameter (char **str, char delimiter1, char delimiter2){
    char* parameter;
    char* temp ;
    remove_leading_whitespace(str);
    parameter = *str;
    temp = *str;
    while(*temp != delimiter1 && *temp != delimiter2 && *temp != '\0'){
        temp++;
    }

    *temp = '\0';
    *str = temp+1;
    temp--;
    while (*temp == ' ' || *temp == '\t'){
        *temp = '\0';
        temp--;
    }
    return parameter;
}

/* Function to get the command from a string */
Command get_command(char *func) {
    Command command;

    if (func == NULL || *func == '\0') {
        command = EMPTY_LINE;
    } else if (strcmp(func, "read_set") == 0) {
        command = READ;
    } else if (strcmp(func, "print_set") == 0) {
        command = PRINT;
    } else if (strcmp(func, "union_set") == 0) {
        command = UNION;
    } else if (strcmp(func, "intersect_set") == 0) {
        command = INTERSECT;
    } else if (strcmp(func, "sub_set") == 0) {
        command = SUB;
    } else if (strcmp(func, "symdiff_set") == 0) {
        command = SYMDIFF;
    } else if (strcmp(func, "stop") == 0) {
        command = STOP;
    } else {
        command = ERROR;
    }
    return command;
}

/* Function to get a set from a string */
int get_set(set *set_array[6],char **str,char expectedNextChar){
    int nextCharNotAsExpectedFlag = 0;
    char nextChar;
    char *set;
    int index;
    if (strlen(*str)<4 && 0<strlen(*str))
        return -1;
    if(**str == '\0')
        return -3;
    nextChar = (*str)[4];
    if(nextChar != expectedNextChar){
        nextCharNotAsExpectedFlag = 1;
    }
    set = get_parameter(str,expectedNextChar, '\0');
    if(nextCharNotAsExpectedFlag){
        set[4] ='\0';
    }
    if(strcmp(set, "SETA") == 0)
        index = 0;
    else if(strcmp(set, "SETB") == 0)
        index = 1;
    else if(strcmp(set, "SETC") == 0)
        index = 2;
    else if(strcmp(set, "SETD") == 0)
        index = 3;
    else if(strcmp(set, "SETE") == 0)
        index = 4;
    else if(strcmp(set, "SETF") == 0)
        index = 5;
    else
        index = -1;
    if(nextCharNotAsExpectedFlag && index>-1 )
        index = -2;
    return index;
}

/* Function to remove all whitespace characters from a string */
void remove_whitespace(char **str) {
    char *temp = *str;
    char *write_ptr = *str;  /* Pointer to write the next non-space character */
    while (*temp != '\n' && *temp != '\0') {
	  if (*temp != ' ' && *temp != '\t') {
            *write_ptr = *temp;
            write_ptr++;
        }
        temp++;
    }
    *write_ptr = '\0';  /* Null-terminate the modified string */
}

/* Function to count the number of elements in a string */
int number_of_elements(char *str){
   int count = 1;
    while (*str != '\0') {
        if (*str == ',') {
            count++;
        }
        str++;
    }
    return count;
}

/* Function to split a string into tokens */
char **splitString(char *str, int numElements) {
    /* Allocate memory for the array of character pointers */
    int index;
    char *token;
    char **tokens = (char **)malloc(numElements * sizeof(char *));
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    /* Tokenize the string and store each token in the array*/
    index = 0;
    token = strtok(str, ",");
    while (token != NULL && index < numElements) {
        tokens[index] = token; /* Store the token directly in the array*/
        index++;
        token = strtok(NULL, ",");
    }

    return tokens;
}

/* Function to check if there are consecutive commas in a string */
int consecutive_commas(char *str){
    char *temp;
    if(*str == ',')
        return 0;
    temp = str++;
    while(*temp != '\0' && *temp != EOF){
        if(*(temp-1) == *temp && *temp == ',')
            return 0;
        temp++;
    }
    return 1;
}

/* Function to check if a string is correctly terminated */
int correctly_terminated(char *str) {
    size_t len = strlen(str); /* Use size_t to avoid narrowing conversion */
    if (len < 2) {
        return 0; /* String is too short to end with "-1"*/
    }

    /* Check if the last two characters are "1" and "-" */
    if (str[len - 1] == '1' && str[len - 2] == '-') {
        return 1;
    } else {
        return 0;
    }
}

/* Function to check if a string represents an integer */
int isInteger(const char *str) {
    /* Handle negative sign if present*/
    if (*str == '-' || *str == '+') {
        str++; /* Move to the next character */
    }
    /* Check each character of the string*/
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0; /* Non-digit character found*/
        }
        str++; /* Move to the next character*/
    }

    return 1; /* All characters are digits, so it's a valid integer*/
}

/* Function to validate if each token in the array is a valid integer */
int valid_set_members(char **tokens, int numElements) {
    int i;
    for (i = 0; i < numElements; i++) {
        if (!isInteger(tokens[i])) {
            return 0; /* Invalid integer found */
        }
    }
    return 1; /* All tokens are valid integers*/
}

/* Function to convert string representations of integers to actual integers */
void convertStringsToInts(char **strArray, int *intArray, int numElements) {
    int i;
    for (i = 0; i < numElements; i++) {
        char *endptr;
        intArray[i] = strtol(strArray[i], &endptr, 10);
    }
}

/* Function to check if all values in an array are within a specified range */
int values_in_range(int *intArray, int numElements){
    int i;
    for (i = 0; i < numElements-1; i++) {
        if(intArray[i]< MIN_ELEMENT_VALUE || intArray[i]>MAX_ELEMENT_VALUE)
            return 0;
    }
    return 1;
}

/* Comparison function for sorting integers */
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/* Function to remove duplicates from a sorted array in-place */
void removeDuplicates(int *array, int size) {
    int uniqueCount;
    int i;
    if (size == 0) {
        return;
    }

    uniqueCount = 1;
    for (i = 1; i < size; i++) {
        if (array[i] != array[i - 1]) {
            array[uniqueCount] = array[i];
            uniqueCount++;
        }
    }
}
