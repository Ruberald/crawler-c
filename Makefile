CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -Lsrc/curl/lib -lcurl

# List of source files
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEADER_DIR = $(SRC_DIR)/curl/include
HEADER_FILES = $(wildcard $(HEADER_DIR)/*.h)

# Output executable name
EXECUTABLE = crawler

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,%.o,$(SRC_FILES))

# Main target
$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: $(SRC_DIR)/%.c $(HEADER_FILES)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	rm -f $(EXECUTABLE) $(OBJ_FILES)

.PHONY: clean
