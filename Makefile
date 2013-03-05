CC	:= gcc
CFLAGS	:= -Wall -pedantic -Werror -Wundef -Winline -Wextra -O2
DESTDIR  =
PREFIX	 = /usr

SOURCES	= elf.c elf-mutate.c
OBJECTS	= $(SOURCES:.c=.o)

all: elf-mutate

elf-mutate: $(OBJECTS)
	$(CC) -o $@ $^

hello:
	echo 'main(){puts("hello");}'|gcc -x c - -o $@

install: elf-mutate
	install -D $< $(DESTDIR)$(PREFIX)/bin/$<

clean:
	rm -f elf-mutate $(OBJECTS) hello hello2
