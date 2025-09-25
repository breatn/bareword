#include "bareword.h"

void print_error(int line, const char* message, const char* detail) {
    if (detail && strlen(detail) > 0) {
        fprintf(stderr, "Error at line %d: %s \"%s\"\n", line, message, detail);
    } else {
        fprintf(stderr, "Error at line %d: %s\n", line, message);
    }
}

int is_valid_identifier(const char* str) {
    if (!str || strlen(str) == 0) return 0;
    
    // First character must be letter or underscore
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    
    // Rest can be letters, digits, or underscores
    for (int i = 1; i < strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    }
    
    return 1;
}

int64_t parse_integer(const char* str) {
    char* endptr;
    int64_t value = strtoll(str, &endptr, 10);
    
    if (*endptr != '\0') {
        return INT64_MIN; // Invalid integer
    }
    
    return value;
}

comparison_t string_to_comparison(const char* str) {
    if (strcmp(str, "==") == 0) return CMP_EQ;
    if (strcmp(str, "!=") == 0) return CMP_NE;
    if (strcmp(str, "<") == 0) return CMP_LT;
    if (strcmp(str, "<=") == 0) return CMP_LE;
    if (strcmp(str, ">") == 0) return CMP_GT;
    if (strcmp(str, ">=") == 0) return CMP_GE;
    return -1; // Invalid comparison
}

opcode_t string_to_opcode(const char* str) {
    if (strcmp(str, "set") == 0) return OP_SET;
    if (strcmp(str, "out") == 0) return OP_OUT;
    if (strcmp(str, "add") == 0) return OP_ADD;
    if (strcmp(str, "sub") == 0) return OP_SUB;
    if (strcmp(str, "mul") == 0) return OP_MUL;
    if (strcmp(str, "div") == 0) return OP_DIV;
    if (strcmp(str, "cmp") == 0) return OP_CMP;
    if (strcmp(str, "if") == 0) return OP_IF;
    if (strcmp(str, "goto") == 0) return OP_GOTO;
    if (strcmp(str, "label") == 0) return OP_LABEL;
    if (strcmp(str, "halt") == 0) return OP_HALT;
    return OP_INVALID;
}

int tokenize_line(const char* line, int line_number, token_t tokens[], int* token_count) {
    *token_count = 0;
    
    // Skip empty lines and comments
    if (strlen(line) == 0) return 1;
    
    char buffer[1024];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    char* token = strtok(buffer, " \t\n\r");
    
    while (token != NULL && *token_count < MAX_TOKENS_PER_LINE) {
        tokens[*token_count].line_number = line_number;
        
        // Handle quoted strings
        if (token[0] == '"') {
            char string_buffer[MAX_STRING_LENGTH] = "";
            int string_pos = 0;
            
            // Remove starting quote
            char* start = token + 1;
            
            // Check if string ends on same token
            char* end_quote = strrchr(start, '"');
            if (end_quote && end_quote > start) {
                *end_quote = '\0';
                strncpy(string_buffer, start, MAX_STRING_LENGTH - 1);
            } else {
                // Multi-token string - get the rest
                strncpy(string_buffer, start, MAX_STRING_LENGTH - 1);
                string_pos = strlen(string_buffer);
                
                token = strtok(NULL, "");  // Get rest of line
                if (token) {
                    if (string_pos < MAX_STRING_LENGTH - 1) {
                        string_buffer[string_pos] = ' ';
                        string_pos++;
                    }
                    
                    char* closing_quote = strchr(token, '"');
                    if (closing_quote) {
                        *closing_quote = '\0';
                        strncat(string_buffer, token, MAX_STRING_LENGTH - string_pos - 1);
                    } else {
                        print_error(line_number, "unterminated string literal", "");
                        return 0;
                    }
                } else {
                    print_error(line_number, "unterminated string literal", "");
                    return 0;
                }
            }
            
            tokens[*token_count].type = TOKEN_STRING;
            strncpy(tokens[*token_count].value, string_buffer, MAX_TOKEN_LENGTH - 1);
            tokens[*token_count].value[MAX_TOKEN_LENGTH - 1] = '\0';
            
            (*token_count)++;
            break; // String token consumes rest of line
        }
        // Handle comparison operators
        else if (string_to_comparison(token) != -1) {
            tokens[*token_count].type = TOKEN_COMPARISON;
            strncpy(tokens[*token_count].value, token, MAX_TOKEN_LENGTH - 1);
            tokens[*token_count].value[MAX_TOKEN_LENGTH - 1] = '\0';
            (*token_count)++;
        }
        // Handle integers
        else if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            if (parse_integer(token) == INT64_MIN) {
                print_error(line_number, "invalid integer format", token);
                return 0;
            }
            
            tokens[*token_count].type = TOKEN_INTEGER;
            strncpy(tokens[*token_count].value, token, MAX_TOKEN_LENGTH - 1);
            tokens[*token_count].value[MAX_TOKEN_LENGTH - 1] = '\0';
            (*token_count)++;
        }
        // Handle opcodes and identifiers
        else {
            opcode_t op = string_to_opcode(token);
            if (op != OP_INVALID) {
                tokens[*token_count].type = TOKEN_OPCODE;
            } else if (is_valid_identifier(token)) {
                tokens[*token_count].type = TOKEN_IDENTIFIER;
            } else {
                print_error(line_number, "invalid token", token);
                return 0;
            }
            
            strncpy(tokens[*token_count].value, token, MAX_TOKEN_LENGTH - 1);
            tokens[*token_count].value[MAX_TOKEN_LENGTH - 1] = '\0';
            (*token_count)++;
        }
        
        token = strtok(NULL, " \t\n\r");
    }
    
    return 1;
}