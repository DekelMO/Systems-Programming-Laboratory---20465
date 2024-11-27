#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "set.h"

/* Function declarations */
char* readString();
void remove_leading_whitespace(char **str);
char *get_parameter (char **str, char delimiter1, char delimiter2);
char *get_func (char **str);

/* Enum for command types */
typedef enum command_t {
    READ,
    PRINT,
    UNION,
    INTERSECT,
    SUB,
    SYMDIFF,
    STOP,
    ERROR,
    EMPTY_LINE
} Command;

void print_instructions();
Command get_command(char *str);
int get_set(set *set_array[6],char **str,char c);
int number_of_elements(char *str);
void remove_whitespace(char **str);
int *to_elements_arr(char **str);
char **splitString(char *str, int numElements);
int correctly_terminated(char *str);
int consecutive_commas(char *str);
int valid_set_members(char **tokens,int numElements);
void convertStringsToInts(char **strArray, int *intArray, int numElements);
int values_in_range(int *intArray, int numElements);
int compare(const void *a, const void *b);
void removeDuplicates(int *array, int size);
