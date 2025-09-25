#include "bareword.h"

int64_t get_variable_value(program_t* program, const char* name) {
    for (int i = 0; i < program->variable_count; i++) {
        if (strcmp(program->variables[i].name, name) == 0) {
            return program->variables[i].value;
        }
    }
    
    // Variable not found - create it with value 0
    if (program->variable_count >= MAX_VARIABLES) {
        fprintf(stderr, "Error: too many variables\n");
        exit(1);
    }
    
    variable_t* var = &program->variables[program->variable_count];
    strncpy(var->name, name, MAX_TOKEN_LENGTH - 1);
    var->name[MAX_TOKEN_LENGTH - 1] = '\0';
    var->value = 0;
    program->variable_count++;
    
    return 0;
}

void set_variable_value(program_t* program, const char* name, int64_t value) {
    for (int i = 0; i < program->variable_count; i++) {
        if (strcmp(program->variables[i].name, name) == 0) {
            program->variables[i].value = value;
            return;
        }
    }
    
    // Variable not found - create it
    if (program->variable_count >= MAX_VARIABLES) {
        fprintf(stderr, "Error: too many variables\n");
        exit(1);
    }
    
    variable_t* var = &program->variables[program->variable_count];
    strncpy(var->name, name, MAX_TOKEN_LENGTH - 1);
    var->name[MAX_TOKEN_LENGTH - 1] = '\0';
    var->value = value;
    program->variable_count++;
}

int64_t resolve_value(program_t* program, const char* arg) {
    // Check if it's a literal integer
    if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
        return parse_integer(arg);
    }
    
    // Otherwise it's a variable
    return get_variable_value(program, arg);
}

int execute_program(program_t* program) {
    int pc = 0; // Program counter
    
    while (pc < program->instruction_count) {
        instruction_t* inst = &program->instructions[pc];
        
        switch (inst->op) {
            case OP_SET: {
                int64_t value = resolve_value(program, inst->args[1]);
                set_variable_value(program, inst->args[0], value);
                break;
            }
            
            case OP_OUT: {
                // Check if argument looks like it should be a string (contains spaces or non-identifier chars)
                char* arg = inst->args[0];
                
                // If it's clearly a string (has spaces) or starts with quote, treat as string
                if (strchr(arg, ' ') || arg[0] == '"' || (!isdigit(arg[0]) && !is_valid_identifier(arg))) {
                    printf("%s\n", arg);
                } else {
                    // Print variable or integer value
                    int64_t value = resolve_value(program, inst->args[0]);
                    printf("%lld\n", (long long)value);
                }
                break;
            }
            
            case OP_ADD: {
                int64_t a = resolve_value(program, inst->args[1]);
                int64_t b = resolve_value(program, inst->args[2]);
                set_variable_value(program, inst->args[0], a + b);
                break;
            }
            
            case OP_SUB: {
                int64_t a = resolve_value(program, inst->args[1]);
                int64_t b = resolve_value(program, inst->args[2]);
                set_variable_value(program, inst->args[0], a - b);
                break;
            }
            
            case OP_MUL: {
                int64_t a = resolve_value(program, inst->args[1]);
                int64_t b = resolve_value(program, inst->args[2]);
                set_variable_value(program, inst->args[0], a * b);
                break;
            }
            
            case OP_DIV: {
                int64_t a = resolve_value(program, inst->args[1]);
                int64_t b = resolve_value(program, inst->args[2]);
                
                if (b == 0) {
                    print_error(inst->line_number, "runtime error: division by zero", "");
                    return 0;
                }
                
                set_variable_value(program, inst->args[0], a / b);
                break;
            }
            
            case OP_CMP: {
                int64_t a = resolve_value(program, inst->args[1]);
                int64_t b = resolve_value(program, inst->args[3]);
                comparison_t op = string_to_comparison(inst->args[2]);
                
                int64_t result = 0;
                switch (op) {
                    case CMP_EQ: result = (a == b); break;
                    case CMP_NE: result = (a != b); break;
                    case CMP_LT: result = (a < b); break;
                    case CMP_LE: result = (a <= b); break;
                    case CMP_GT: result = (a > b); break;
                    case CMP_GE: result = (a >= b); break;
                    default:
                        print_error(inst->line_number, "invalid comparison operator", inst->args[2]);
                        return 0;
                }
                
                set_variable_value(program, inst->args[0], result);
                break;
            }
            
            case OP_IF: {
                int64_t condition = get_variable_value(program, inst->args[0]);
                
                if (condition != 0) {
                    // Jump to label
                    int target = find_label(program, inst->args[2]);
                    if (target == -1) {
                        print_error(inst->line_number, "undefined label", inst->args[2]);
                        return 0;
                    }
                    pc = target;
                    continue;
                }
                break;
            }
            
            case OP_GOTO: {
                int target = find_label(program, inst->args[0]);
                if (target == -1) {
                    print_error(inst->line_number, "undefined label", inst->args[0]);
                    return 0;
                }
                pc = target;
                continue;
            }
            
            case OP_LABEL:
                // Labels are no-ops during execution
                break;
                
            case OP_HALT:
                return 1;
                
            default:
                print_error(inst->line_number, "unknown instruction", "");
                return 0;
        }
        
        pc++;
    }
    
    // Program ended without halt
    print_error(0, "program ended without halt instruction", "");
    return 0;
}