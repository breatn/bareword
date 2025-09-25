# Bareword Programming Language Makefile
# Compiles the Bareword interpreter with ANSI C compatibility

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -g
TARGET = bareword
SOURCES = main.c lexer.c parser.c validator.c executor.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.c bareword.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install to /usr/local/bin (requires sudo)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Remove from /usr/local/bin (requires sudo)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run tests
test: $(TARGET)
	@echo "Running test programs..."
	./$(TARGET) examples/hello.bw
	./$(TARGET) examples/math.bw
	./$(TARGET) examples/conditional.bw

# Create example programs
examples: examples/hello.bw examples/math.bw examples/conditional.bw

examples/hello.bw:
	@mkdir -p examples
	@echo 'out "Hello world"' > $@
	@echo 'halt' >> $@

examples/math.bw:
	@mkdir -p examples
	@echo 'set x 10' > $@
	@echo 'set y 20' >> $@
	@echo 'add sum x y' >> $@
	@echo 'out sum' >> $@
	@echo 'halt' >> $@

examples/conditional.bw:
	@mkdir -p examples
	@echo 'set x 5' > $@
	@echo 'set y 10' >> $@
	@echo 'cmp cond x < y' >> $@
	@echo 'if cond goto smaller' >> $@
	@echo 'out "x is not smaller"' >> $@
	@echo 'goto end' >> $@
	@echo 'label smaller' >> $@
	@echo 'out "x is smaller"' >> $@
	@echo 'label end' >> $@
	@echo 'halt' >> $@

# Debug build
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(TARGET)

# Check for memory leaks with valgrind (if available)
memcheck: $(TARGET) examples
	@which valgrind > /dev/null || (echo "valgrind not found, skipping memory check" && exit 0)
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) examples/hello.bw

# Help
help:
	@echo "Bareword Language Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the bareword interpreter (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  install    - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall  - Remove from /usr/local/bin (requires sudo)"
	@echo "  test       - Build and run test programs"
	@echo "  examples   - Create example .bw programs"
	@echo "  debug      - Build with debug symbols"
	@echo "  memcheck   - Run with valgrind memory checking"
	@echo "  help       - Show this help message"

.PHONY: all clean install uninstall test examples debug memcheck help