# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Werror

# Target Binary Name
TARGET = ysh

# Source Files
SRCS = ysh.c

# Default Rule
all: $(TARGET)

# Build Rule
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean Rule
clean:
	rm -f $(TARGET) *.o