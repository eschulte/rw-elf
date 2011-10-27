#include <stdio.h>
#include <stdlib.h>

#define ELF32 1
#define ELF64 2
#define READ(buf,x,y) int_from_bytes(buf,(16 + (class * x)),(class * y))
#define CREAD(buf,x,y) int_from_bytes(buf,(16 + (class * x)),y)
#define CLASS(buf)   buf[4]
#define TYPE(buf)    READ(buf, 0,  1)
#define MACHINE(buf) READ(buf, 1,  1)
#define VERSION(buf) READ(buf, 2,  1)
#define ENTRY(buf)   READ(buf, 4,  2)
#define PHOFF(buf)   READ(buf, 8,  2)
#define SHOFF(buf)   READ(buf, 12, 2)
#define FLAGS(buf)   CREAD(buf, 16, 2)
#define EH_SZ(buf)   CREAD(buf, 18, 1)
#define PH_E_SZ(buf) CREAD(buf, 19, 1)
#define PH_NUM(buf)  CREAD(buf, 20, 1)
#define SH_E_SZ(buf) CREAD(buf, 21, 1)
#define SH_NUM(buf)  CREAD(buf, 22, 1)
#define SH_SRND(buf) CREAD(buf, 23, 1)

int class;
char *read_raw(char *path);
void check_magic(char *buf);
int elf_class(char *buf);
int int_from_bytes(char *buf, int pos, int num);
void print_header_info(char *buf);
int text_section_header(char *buf);

int main(int argc, char *argv[]){
  char *buf = read_raw(argv[1]);

  /* sanity check */
  check_magic(buf);
  class = CLASS(buf);
  if(!((class == 1) || (class == 2))){
    printf("invalid elf class\n");
    return 1;
  }

  /* print out header information */
  print_header_info(buf);

  /* find the text section */
  

  /* return the text section */

  /* update the text section */

  return 0;
}

int text_section_header(char *buf){
  /* find the section headers */
  int shoff = SHOFF(buf);
  /* find the section names */
  /* move through section headers until found .text section header */

}

void print_header_info(char *buf){
  switch (class){
  case ELF32: printf("class\t32-bit\n"); break;
  case ELF64: printf("class\t64-bit\n"); break;
  }

  /* print out some information about the elf file */
  printf("type\t%d\n", TYPE(buf));
  printf("machine\t%d\n", MACHINE(buf));
  printf("version\t%d\n", VERSION(buf));
  printf("entry\t%d\n", ENTRY(buf));
  printf("phoff\t%d\n", PHOFF(buf));
  printf("shoff\t%d\n", SHOFF(buf));
  printf("flags\t%d\n", FLAGS(buf));
  printf("eh_sz\t%d\n", EH_SZ(buf));
  printf("ph_e_sz\t%d\n", PH_E_SZ(buf));
  printf("ph_num\t%d\n", PH_NUM(buf));
  printf("sh_e_sz\t%d\n", SH_E_SZ(buf));
  printf("sh_num\t%d\n", SH_NUM(buf));
  printf("sh_srnd\t%d\n", SH_SRND(buf));
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
