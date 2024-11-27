#pragma once

#define MEMORY_NOT_ALLOCATED_ERROR_CODE 0
#define HASH_TABLE_INSERT_CONTAINS_KEY_ERROR_CODE 0
#define HASH_TABLE_KEY_DOESNT_EXIST_ERROR_CODE 0
#define MAIN_ERROR_CODE 0
#define HASH_TABLE_INSERT_EMPTY_KEY_ERROR_CODE 0
#define HASH_TABLE_INSERT_EMPTY_VALUE_ERROR_CODE 0

#define NUMBER_OF_LINE_EXCEED_MAX_LINES "Lines exceed the maximum amount\n"
#define MACRO_ALREADY_EXISTS_ERROR_MESSAGE "This macro already exists. Exiting...\n"
#define HASH_TABLE_KEY_DOESNT_EXIST_ERROR_MESSAGE "This key doesn't exist in the hash table.\n"
#define MEMORY_NOT_ALLOCATED_SUCCESSFULLY_ERROR_MESSAGE "Memory wasn't allocated successfully.\n"
#define LABEL_DECLARED_EXTERN_AND_ENTRY_ERROR_MESSAGE "Entry label already declared as extern label.\n"
#define LABEL_DECLARED_ENTRY_AND_EXTERN_ERROR_MESSAGE "Extern label already declared as entry label.\n"
#define LABEL_DECLARED_EXTERN_ERROR_MESSAGE "Label already declared as extern label.\n"
#define LABEL_DECLARED_ENTRY_ERROR_MESSAGE "Label already declared as entry label.\n"
#define REDEFINITION_OF_LABEL_ERROR_MESSAGE "Label already defined.\n"
#define LABEL_UNDEFINED_ERROR_MESSAGE "Label wasn't defined.\n"
#define ILLEGAL_TEXT_AFTER_MACRO_NAME_ERROR_MESSAGE "Extra characters found after macro name\n"
#define ILLEGAL_TEXT_AFTER_MACRO_END_ERROR_MESSAGE "Extra characters found after macro end\n"
#define MACRO_NAME_ILLEGAL_ERROR_MESSAGE "Macro name is illegal.\n"
#define SAME_MACRO_LABEL_NAME_ERROR_MESSAGE "Macro and label share the same name\n"

#define UNKNOWN_CODE -1
#define MOV_CODE 0
#define CMP_CODE 1
#define ADD_CODE 2
#define SUB_CODE 3
#define LEA_CODE 4
#define CLR_CODE 5
#define NOT_CODE 6
#define INC_CODE 7
#define DEC_CODE 8
#define JMP_CODE 9
#define BNE_CODE 10
#define RED_CODE 11
#define PRN_CODE 12
#define JSR_CODE 13
#define RTS_CODE 14
#define STOP_CODE 15

#define NUM_REGISTERS 8
#define MAX_WORD_LENGTH 81
#define HASH_TABLE_SIZE 10
#define NUM_BITS 15

#define SHIFTS_FOR_DEST 3
#define SHIFTS_FOR_OPCODE 11
#define SHIFTS_FOR_SRC_REGISTER 10
#define SHIFTS_FOR_SRC_REGISTER_ADDRESS 9
#define SHIFTS_FOR_SRC_LABEL 8
#define SHIFTS_FOR_SRC_DIRECT 7
#define SHIFTS_FOR_DEST_REGISTER 6
#define SHIFTS_FOR_DEST_REGISTER_ADDRESS 5
#define SHIFTS_FOR_DEST_LABEL 4
#define SHIFTS_FOR_DEST_DIRECT 3
#define SHIFTS_FOR_FIRST_PARAM_REGISTER 6
#define SHIFTS_FOR_SECOND_PARAM_REGISTER 3


#define BIT 1


/* E,R,A operands */
#define E 1
#define R 2
#define A 4

#define MAX_LINES 3995
#define DEF_ARRAY_SIZE 10
#define READ_MODE "r"
#define WRITE_MODE "w"
#define MACRO_KEYWORD "macr"
#define END_MACRO_KEYWORD "endmacr"

#define DATA_STORE_COMMAND ".data"
#define STRING_STORE_COMMAND ".string"
#define ENTRY_COMMAND ".entry"
#define EXTERN_COMMAND ".extern"
#define STRING_STORE_COMMAND_LENGTH 7

enum macroState {
    NOT_IN_MACRO, IN_MACRO_BODY, IN_MACRO_NAME, END_MACRO
};

extern const char *REGISTERS_NAMES[8];
