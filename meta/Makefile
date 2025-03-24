IDIR=./include
TARGET=bin/main

CC=gcc
CFLAGS=-I$(IDIR)
DEPS=$(wildcard $(IDIR)/**/*.h)
SRCS := $(shell find src -name '*.c')
OBJS=$(patsubst src/%.c, build/%.o, $(SRCS))
TEMPLATES := $(shell find templates -name '*.c')

.PHONY: debug release clean

debug: CFLAGS += -g -O0 -Wall -Wextra
debug: $(TARGET)

release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

templates/%.c:
	$(CC) -c -o templates/$@.exe $<

build/%.o: src/%.c $(DEPS)
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


$(TARGET): $(TEMPS) $(OBJS)
	mkdir -p bin
	$(patsubst templates/%.c, ./templates/%.exe, $(TEMPLATES))
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf build bin