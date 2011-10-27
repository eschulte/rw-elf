#include <stdio.h>
#include <stdlib.h>

#define ELF32 1
#define ELF64 2
#define READ(buf,x,y) int_from_bytes(buf,(16 + (class * x)),(class * y))
#define CLASS(buf)   buf[4]
#define TYPE(buf)    READ(buf, 0,  1)
#define MACHINE(buf) READ(buf, 1,  1)
#define VERSION(buf) READ(buf, 2,  1)
#define ENTRY(buf)   READ(buf, 4,  2)
#define PHOFF(buf)   READ(buf, 8,  2)
#define SHOFF(buf)   READ(buf, 12, 2)

int class;
char *read_raw(char *path);
void check_magic(char *buf);
int elf_class(char *buf);
int int_from_bytes(char *buf, int pos, int num);

int main(int argc, char *argv[]){
  char *buf = read_raw(argv[1]);

  /* sanity check */
  check_magic(buf);
  class = CLASS(buf);
  switch (class){
  case ELF32: printf("class\t32-bit\n"); break;
  case ELF64: printf("class\t64-bit\n"); break;
  default: printf("invalid elf class\n"); return 1;
  }

  /* print out some information about the elf file */
  printf("type\t%d\n", TYPE(buf));
  printf("machine\t%d\n", MACHINE(buf));
  printf("version\t%d\n", VERSION(buf));
  printf("entry\t%d\n", ENTRY(buf));
  printf("phoff\t%d\n", PHOFF(buf));
  printf("shoff\t%d\n", SHOFF(buf));

  return 0;
}

int int_from_bytes(char *buf, int pos, int num){
  char tmp[num];
  int i, acc = 0;
  for(i=0;i<num;i++) tmp[i] = buf[(pos + i)];
  for(i=0;i<num;i++) acc += tmp[i] << (8 * i);
  return acc;
}

int elf_class(char *buf){
  return buf[4];
}

void check_magic(char *buf){
  const char *magic = "ELF";
  int i;
  for(i=0; i<4; i++)
    if(!(buf[i] == magic[i]))
      printf("ELF Magic number test failed.");
}

char *read_raw(char *path){
  FILE *file;
  char *buf;
  unsigned long length;

  /* open file */
  file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "Unable to open file %s", path);
    return;
  }
	
  /* find file length */
  fseek(file, 0, SEEK_END);
  length=ftell(file);
  fseek(file, 0, SEEK_SET);

  /* allocate memory */
  buf= (char *) malloc(length+1);

  /* read file */
  fread(buf, length, 1, file);
  fclose(file);

  return buf;
}
