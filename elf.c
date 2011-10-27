#include <stdio.h>
#include <stdlib.h>

int class;
#define ELF32 1
#define ELF64 2
#define READ(buf,x,y) int_from_bytes(buf,(16 + (class * x)),(class * y))

char *read_raw(char *path);
int int_from_bytes(char *buf, int pos, int num);

void check_magic(char *buf);
int elf_class(char *buf);
int elf_type(char *buf); // 2
int elf_machine(char *buf); // 2
int elf_version(char *buf); // 4
int entry(char *buf);
int phoff(char *buf);
int shoff(char *buf);

void main(int argc, char *argv[]){
  char *buf = read_raw(argv[1]);

  check_magic(buf);
  class = elf_class(buf);
  switch (class){
  case ELF32: printf("32-bit elf file.\n"); break;
  case ELF64: printf("64-bit elf file.\n"); break;
  default: printf("invalide elf class\n");
  }

  printf("elf_type=%d\n", elf_type(buf));
  printf("elf_machine=%d\n", elf_machine(buf));
  printf("elf_version=%d\n", elf_version(buf));
  printf("entry=%d\n", entry(buf));
  printf("phoff=%d\n", phoff(buf));
  printf("shoff=%d\n", shoff(buf));
  printf("%d\n", READ(buf, 4, 2));
  printf("%d\n", READ(buf, 4, 2));
}

int shoff(char *buf){
  return READ(buf, 12, 2);
}

int phoff(char *buf){
  return READ(buf, 8, 2);
}

int entry(char *buf){
  return READ(buf, 4, 2);
}

int elf_version(char *buf){
  return READ(buf, 2, 1);
}

int elf_machine(char *buf){
  return READ(buf, 1, 1);
}

int elf_type(char *buf){
  return READ(buf, 0, 1);
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

int int_from_bytes(char *buf, int pos, int num){
  char tmp[num];
  int i, acc = 0;
  for(i=0;i<num;i++) tmp[i] = buf[(pos + i)];
  for(i=0;i<num;i++) acc += tmp[i] << (8 * i);
  return acc;
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
