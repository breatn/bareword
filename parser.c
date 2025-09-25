#include "bareword.h"

int parse_program(const char* filename, program_t* program) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open file '%s'\n", filename);
        return 0;
    }
    
    program->instruction_count = 0;
    program->variable_count = 0;
    program->label_count = 0;
    
    char line[1024];
    int line_number = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        
        // Skip empty lines and whitespace-only lines
        char* trimmed = line;
        while (isspace(*trimmed)) trimmed++;
        if (strlen(trimmed) == 0) continue;
        
        // Tokenize the line
        token_t tokens[MAX_TOKENS_PER_LINE];
        int token_count;
        
        if (!tokenize_line(trimmed, line_number, tokens, &token_count)) {
            fclose(file);
            return 0;
        }
        
        if (token_count == 0) continue;
        
        // First token must be an opcode
        if (tokens[0].type != TOKEN_OPCODE) {
            print_error(line_number, "expected opcode at start of line", tokens[0].value);
            fclose(file);
            return 0;
        }
        
        // Parse instruction
        instruction_t* inst = &program->instructions[program->instruction_count];
        inst->op = string_to_opcode(tokens[0].value);
        inst->arg_count = token_count - 1;
        inst->line_number = line_number;
        
        // Copy arguments
        for (int i = 1; i < token_count; i++) {
            strncpy(inst->args[i-1], tokens[i].value, MAX_TOKEN_LENGTH - 1);
            inst->args[i-1][MAX_TOKEN_LENGTH - 1] = '\0';
        }
        
        // Validate instruction format
        switch (inst->op) {
            case OP_SET:
                if (inst->arg_count != 2) {
                    print_error(line_number, "set requires exactly 2 arguments", "set variable value");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "set requires variable name as first argument", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                if (tokens[2].type != TOKEN_INTEGER && tokens[2].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "set requires integer or variable as second argument", tokens[2].value);
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_OUT:
                if (inst->arg_count != 1) {
                    print_error(line_number, "out requires exactly 1 argument", "out value");
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
                if (inst->arg_count != 3) {
                    print_error(line_number, "arithmetic operations require exactly 3 arguments", "op result a b");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "result must be a variable name", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_CMP:
                if (inst->arg_count != 4) {
                    print_error(line_number, "cmp requires exactly 4 arguments", "cmp result a op b");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "result must be a variable name", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                if (tokens[3].type != TOKEN_COMPARISON) {
                    print_error(line_number, "invalid comparison operator", tokens[3].value);
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_IF:
                if (inst->arg_count != 3) {
                    print_error(line_number, "if requires exactly 3 arguments", "if condition goto label");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "condition must be a variable", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                if (strcmp(tokens[2].value, "goto") != 0) {
                    print_error(line_number, "if must be followed by 'goto'", tokens[2].value);
                    fclose(file);
                    return 0;
                }
                if (tokens[3].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "goto requires a label name", tokens[3].value);
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_GOTO:
                if (inst->arg_count != 1) {
                    print_error(line_number, "goto requires exactly 1 argument", "goto label");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "goto requires a label name", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_LABEL:
                if (inst->arg_count != 1) {
                    print_error(line_number, "label requires exactly 1 argument", "label name");
                    fclose(file);
                    return 0;
                }
                if (tokens[1].type != TOKEN_IDENTIFIER) {
                    print_error(line_number, "label requires a name", tokens[1].value);
                    fclose(file);
                    return 0;
                }
                
                // Register the label
                if (program->label_count >= MAX_LABELS) {
                    print_error(line_number, "too many labels", "");
                    fclose(file);
                    return 0;
                }
                
                label_t* label = &program->labels[program->label_count];
                strncpy(label->name, tokens[1].value, MAX_TOKEN_LENGTH - 1);
                label->name[MAX_TOKEN_LENGTH - 1] = '\0';
                label->instruction_index = program->instruction_count;
                program->label_count++;
                break;
                
            case OP_HALT:
                if (inst->arg_count != 0) {
                    print_error(line_number, "halt takes no arguments", "");
                    fclose(file);
                    return 0;
                }
                break;
                
            case OP_INVALID:
                print_error(line_number, "invalid opcode", tokens[0].value);
                fclose(file);
                return 0;
        }
        
        program->instruction_count++;
        
        if (program->instruction_count >= MAX_LINES) {
            print_error(line_number, "program too long", "");
            fclose(file);
            return 0;
        }
    }
    
    fclose(file);
    return 1;
}