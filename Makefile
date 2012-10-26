CC := gcc
SHELL := /bin/bash

SOURCES	= elf.c rw-elf.c
OBJECTS	= $(SOURCES:.c=.o)

all: rw-elf

%: %.o
	$(CC) -o $@ $<

rw-elf: rw-elf.o elf.o
	$(CC) -o $@ $^

check: hello rw-elf
	./rw-elf ./hello ./hello2; \
	chmod +x hello2; \
	diff     hello      hello2  && echo "[FAIL] same binary"; \
	diff <(./hello) <(./hello2) || echo "[FAIL] different output"; \
	echo "[PASS] same output different elf files";

clean:
	rm -f rw-elf $(OBJECTS) hello hello2
