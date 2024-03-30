CC = gcc
CFLAGS = -g -Wall -I./include

SRC_DIR = ./src
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)

INCLUDE_DIR = ./include
HEADER = $(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(INCLUDE_DIR)/**/*.h)

OUTPUT = fileManager

all: $(OUTPUT)

$(OUTPUT): $(SRCS) $(HEADER)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

docs:
	doxygen Doxyfile


clean:
	rm -f $(OUTPUT)

.PHONY: clean