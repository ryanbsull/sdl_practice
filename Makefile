CC = gcc
CFLAGS = -lSDL2 -g
SRC_FILES := $(wildcard src/*.c)
OUTPUT_FILES := $(patsubst src/%.c,build/%,$(SRC_FILES))

all: $(OUTPUT_FILES)

build/%: src/%.c
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	rm -rf $(wildcard $(OUTPUT_FILES)*)
