#include "bareword.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <program.bw>\n", program_name);
    printf("  Execute a Bareword program\n\n");
    printf("Bareword Language Reference:\n");
    printf("  set var value    - Set variable to value\n");
    printf("  out value        - Output value or string\n");
    printf("  add res a b      - Set res = a + b\n");
    printf("  sub res a b      - Set res = a - b\n");
    printf("  mul res a b      - Set res = a * b\n");
    printf("  div res a b      - Set res = a / b\n");
    printf("  cmp res a op b   - Compare a and b (op: ==, !=, <, <=, >, >=)\n");
    printf("  if cond goto lbl - Jump to label if condition is true\n");
    printf("  goto lbl         - Jump to label\n");
    printf("  label name       - Define a label\n");
    printf("  halt             - Stop program execution\n\n");
    printf("Examples:\n");
    printf("  set x 10\n");
    printf("  out \"Hello world\"\n");
    printf("  cmp result x > 5\n");
    printf("  if result goto end\n");
    printf("  label end\n");
    printf("  halt\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    
    // Check file extension
    const char* ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".bw") != 0) {
        fprintf(stderr, "Warning: Bareword programs should have .bw extension\n");
    }
    
    program_t program;
    
    printf("Bareword Interpreter v1.0\n");
    printf("Parsing '%s'...\n", filename);
    
    // Parse the program
    if (!parse_program(filename, &program)) {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    
    printf("Parsed %d instructions, %d labels\n", program.instruction_count, program.label_count);
    
    // Validate the program
    if (!validate_program(&program)) {
        fprintf(stderr, "Validation failed.\n");
        return 1;
    }
    
    printf("Validation passed. Executing...\n\n");
    
    // Execute the program
    if (!execute_program(&program)) {
        fprintf(stderr, "\nExecution failed.\n");
        return 1;
    }
    
    printf("\nProgram completed successfully.\n");
    return 0;
}