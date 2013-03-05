CC	:= gcc
CFLAGS	:= -Wall -pedantic -Werror -Wundef -Winline -Wextra -O2
DESTDIR  =
PREFIX	 = /usr

SOURCES	= elf.c rw-elf.c
OBJECTS	= $(SOURCES:.c=.o)

all: rw-elf

rw-elf: $(OBJECTS)
	$(CC) -o $@ $^

hello:
	echo 'main(){puts("hello");}'|gcc -x c - -o $@

install: rw-elf
	install -D $< $(DESTDIR)$(PREFIX)/bin/$<

clean:
	rm -f rw-elf $(OBJECTS) hello hello2
