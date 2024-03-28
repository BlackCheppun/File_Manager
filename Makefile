CC = gcc
CFLAGS = -Wall -I./include

SRC_DIR = ./src
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)

INCLUDE_DIR = ./include
HEADER = $(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(INCLUDE_DIR)/**/*.h)

OUTPUT = fileManager

$(OUTPUT): $(SRCS) $(HEADER)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

docs:
	doxygen


clean:
	rm -f $(OUTPUT)

all: $(OUTPUT)
.PHONY: clean