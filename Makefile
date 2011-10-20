CC=gcc
LIBELF_PATH=/usr/local/src/elftoolchain/
LIBELF_LIB=/usr/lib/libelf.a

all: simple-libelf extend-a-section hello

simple-libelf: simple-libelf.c
	$(CC) -o $@ -I$(LIBELF_PATH)common -I$(LIBELF_PATH)libelf/ $^ $(LIBELF_LIB)

extend-a-section: extending-a-section.c
	$(CC) -o $@ -I$(LIBELF_PATH)common -I$(LIBELF_PATH)libelf/ $^ $(LIBELF_LIB)

hello: hello.c
	$(CC) -o $@ $^

clean:
	rm -f hello extend-a-section simple-libelf
