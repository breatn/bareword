#include "bareword.h"

int find_label(program_t* program, const char* name) {
    for (int i = 0; i < program->label_count; i++) {
        if (strcmp(program->labels[i].name, name) == 0) {
            return program->labels[i].instruction_index;
        }
    }
    return -1;
}

int validate_program(program_t* program) {
    // Check for duplicate labels
    for (int i = 0; i < program->label_count; i++) {
        for (int j = i + 1; j < program->label_count; j++) {
            if (strcmp(program->labels[i].name, program->labels[j].name) == 0) {
                print_error(0, "duplicate label", program->labels[i].name);
                return 0;
            }
        }
    }
    
    // Validate all instructions
    for (int i = 0; i < program->instruction_count; i++) {
        instruction_t* inst = &program->instructions[i];
        
        switch (inst->op) {
            case OP_IF:
                // Check if the label exists
                if (find_label(program, inst->args[2]) == -1) {
                    print_error(inst->line_number, "undefined label", inst->args[2]);
                    return 0;
                }
                break;
                
            case OP_GOTO:
                // Check if the label exists
                if (find_label(program, inst->args[0]) == -1) {
                    print_error(inst->line_number, "undefined label", inst->args[0]);
                    return 0;
                }
                break;
                
            case OP_CMP:
                // Validate comparison operator
                if (string_to_comparison(inst->args[2]) == -1) {
                    print_error(inst->line_number, "invalid comparison operator", inst->args[2]);
                    return 0;
                }
                break;
                
            case OP_DIV:
                // Check for division by zero with literal values
                if (strcmp(inst->args[2], "0") == 0) {
                    print_error(inst->line_number, "division by zero", "");
                    return 0;
                }
                break;
                
            default:
                break;
        }
    }
    
    // Check that program has at least one halt instruction
    int has_halt = 0;
    for (int i = 0; i < program->instruction_count; i++) {
        if (program->instructions[i].op == OP_HALT) {
            has_halt = 1;
            break;
        }
    }
    
    if (!has_halt) {
        print_error(0, "program must contain at least one 'halt' instruction", "");
        return 0;
    }
    
    // Validate identifier names
    for (int i = 0; i < program->instruction_count; i++) {
        instruction_t* inst = &program->instructions[i];
        
        for (int j = 0; j < inst->arg_count; j++) {
            char* arg = inst->args[j];
            
            // Skip string literals (handled by parser already)
            if (inst->op == OP_OUT && j == 0) {
                continue; // OUT instruction can take strings, variables, or integers
            }
            
            // Skip integers
            if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
                continue;
            }
            
            // Skip comparison operators
            if (string_to_comparison(arg) != -1) {
                continue;
            }
            
            // Skip 'goto' keyword
            if (strcmp(arg, "goto") == 0) {
                continue;
            }
            
            // Validate identifier
            if (!is_valid_identifier(arg)) {
                print_error(inst->line_number, "invalid identifier", arg);
                return 0;
            }
        }
    }
    
    return 1;
}