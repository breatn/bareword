#ifndef BAREWORD_H
#define BAREWORD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_TOKEN_LENGTH 256
#define MAX_TOKENS_PER_LINE 16
#define MAX_LINES 1024
#define MAX_VARIABLES 256
#define MAX_LABELS 64
#define MAX_STRING_LENGTH 512

typedef enum {
    TOKEN_OPCODE,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_COMPARISON
} token_type_t;

typedef struct {
    token_type_t type;
    char value[MAX_TOKEN_LENGTH];
    int line_number;
} token_t;

typedef enum {
    OP_SET,     // set var value
    OP_OUT,     // out value
    OP_ADD,     // add result a b
    OP_SUB,     // sub result a b
    OP_MUL,     // mul result a b
    OP_DIV,     // div result a b
    OP_CMP,     // cmp result a op b
    OP_IF,      // if condition goto label
    OP_GOTO,    // goto label
    OP_LABEL,   // label name
    OP_HALT,    // halt
    OP_INVALID
} opcode_t;

typedef enum {
    CMP_EQ,     // ==
    CMP_NE,     // !=
    CMP_LT,     // <
    CMP_LE,     // <=
    CMP_GT,     // >
    CMP_GE      // >=
} comparison_t;

typedef struct {
    opcode_t op;
    char args[4][MAX_TOKEN_LENGTH];
    int arg_count;
    int line_number;
} instruction_t;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    int64_t value;
} variable_t;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    int instruction_index;
} label_t;

typedef struct {
    instruction_t instructions[MAX_LINES];
    int instruction_count;
    variable_t variables[MAX_VARIABLES];
    int variable_count;
    label_t labels[MAX_LABELS];
    int label_count;
} program_t;

// Function declarations
void print_error(int line, const char* message, const char* detail);
int tokenize_line(const char* line, int line_number, token_t tokens[], int* token_count);
opcode_t string_to_opcode(const char* str);
comparison_t string_to_comparison(const char* str);
int parse_program(const char* filename, program_t* program);
int validate_program(program_t* program);
int execute_program(program_t* program);
int64_t get_variable_value(program_t* program, const char* name);
void set_variable_value(program_t* program, const char* name, int64_t value);
int find_label(program_t* program, const char* name);
int is_valid_identifier(const char* str);
int64_t parse_integer(const char* str);

#endif // BAREWORD_H