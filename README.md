# Bareword Programming Language

Bareword is the simplest possible programming language that is still readable by humans, designed above all to be easy for AI models to write without mistakes. Every design choice prioritizes AI-friendliness, minimal tokens, fast execution, and detailed debugging.

## Language Features

- **Minimal Tokens**: Uses the absolute minimum number of tokens
- **Simple Syntax**: Each line is a single instruction: `verb arg1 arg2 ...`
- **No Punctuation**: No braces, semicolons, parentheses - only quoted strings
- **AI-Friendly**: Nearly impossible to produce invalid syntax due to misplaced punctuation
- **Fast Execution**: Compiled to efficient internal representation
- **Detailed Errors**: Clear error messages with line numbers and suggestions

## Core Vocabulary

| Instruction | Arguments | Description |
|-------------|-----------|-------------|
| `set` | var value | Set variable to value |
| `out` | value | Output value or string |
| `add` | result a b | Set result = a + b |
| `sub` | result a b | Set result = a - b |
| `mul` | result a b | Set result = a * b |
| `div` | result a b | Set result = a / b |
| `cmp` | result a op b | Compare a and b (op: ==, !=, <, <=, >, >=) |
| `if` | condition goto label | Jump to label if condition is true |
| `goto` | label | Jump to label |
| `label` | name | Define a label |
| `halt` | | Stop program execution |

## Examples

### Hello World
```
out "Hello world"
halt
```

### Simple Math
```
set x 10
set y 20
add sum x y
out sum
halt
```

### Conditional Logic
```
set x 5
set y 10
cmp cond x < y
if cond goto smaller
out "x is not smaller"
goto end
label smaller
out "x is smaller" 
label end
halt
```

## Building

```bash
make
```

## Usage

```bash
./bareword program.bw
```

## Implementation

The compiler/interpreter consists of four main phases:

1. **Lexer** (`lexer.c`) - Tokenizes input lines, handles strings and numbers
2. **Parser** (`parser.c`) - Validates syntax and builds instruction list
3. **Validator** (`validator.c`) - Performs semantic checks and label resolution
4. **Executor** (`executor.c`) - Runs the compiled program efficiently

## Error Handling

All errors include line numbers and helpful suggestions:

```
Error at line 3: invalid opcode "ot". Did you mean "out"?
Error at line 5: undefined label "end_loop"
Error at line 7: division by zero
```

## Design Principles

1. **AI-First**: Every syntax choice optimizes for AI model generation
2. **Minimal Tokens**: Reduce cognitive load and parsing complexity
3. **Zero Ambiguity**: One correct way to write each construct
4. **Fast Feedback**: Immediate, actionable error messages
5. **Portable**: Clean ANSI C, works on macOS and Linux

## File Structure

- `bareword.h` - Main header with data structures
- `lexer.c` - Tokenization and basic validation
- `parser.c` - Syntax parsing and instruction building  
- `validator.c` - Semantic validation and optimization
- `executor.c` - Runtime execution engine
- `main.c` - Command-line interface
- `Makefile` - Build system
- `examples/` - Sample programs

## License

Public domain - use freely for any purpose.