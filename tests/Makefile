CC=gcc
CFLAGS=-Wall -Wextra -I../include
LDFLAGS=

# Obtenir tous les fichiers source du projet principal (sauf main.c)
SRC_DIR=../src
SRCS=$(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c))
OBJS=$(SRCS:.c=.o)

# Fichiers de test
TEST_SRCS=tests.c
TEST_OBJS=$(TEST_SRCS:.c=.o)

# Nom de l'exécutable
TARGET=run_tests

all: $(TARGET)

$(TARGET): $(TEST_OBJS) $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_OBJS)
	rm -f $(OBJS)
	rm -f test_partition.bin

test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test 