CC=gcc
LIBELF_PATH=/usr/local/src/elftoolchain/
LIBELF_LIB=/usr/lib/libelf.a

all: funcs simple-libelf edit-text hello

funcs: funcs.c
	$(CC) -o $@ -I$(LIBELF_PATH)common -I$(LIBELF_PATH)libelf/ $^ $(LIBELF_LIB)

simple-libelf: simple-libelf.c
	$(CC) -o $@ -I$(LIBELF_PATH)common -I$(LIBELF_PATH)libelf/ $^ $(LIBELF_LIB)

edit-text: edit-text.c
	$(CC) -o $@ -I$(LIBELF_PATH)common -I$(LIBELF_PATH)libelf/ $^ $(LIBELF_LIB)

hello: hello.c
	$(CC) -o $@ $^

clean:
	rm -f hello edit-text simple-libelf
