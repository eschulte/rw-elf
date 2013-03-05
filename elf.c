#include "elf.h"

/* Global State */
int class;

/* Elf Class */
#define ELF32 1
#define ELF64 2

/* Elf Header */
#define   CLASS(buf) buf[4]
#define    TYPE(buf) int_from_bytes(buf, 16,                       2)
#define MACHINE(buf) int_from_bytes(buf, 18,                       2)
#define VERSION(buf) int_from_bytes(buf, 20,                       4)
#define   ENTRY(buf) int_from_bytes(buf, 24,                       (class * 4))
#define   PHOFF(buf) int_from_bytes(buf, 24 +       (class * 4),   (class * 4))
#define   SHOFF(buf) int_from_bytes(buf, 24 +       (class * 8),   (class * 4))
#define   FLAGS(buf) int_from_bytes(buf, 24 +       (class * 12),  4)
#define   EH_SZ(buf) int_from_bytes(buf, 24 + (4  + (class * 12)), 2)
#define PH_E_SZ(buf) int_from_bytes(buf, 24 + (6  + (class * 12)), 2)
#define  PH_NUM(buf) int_from_bytes(buf, 24 + (8  + (class * 12)), 2)
#define SH_E_SZ(buf) int_from_bytes(buf, 24 + (10 + (class * 12)), 2)
#define  SH_NUM(buf) int_from_bytes(buf, 24 + (12 + (class * 12)), 2)
#define SH_SRND(buf) int_from_bytes(buf, 24 + (14 + (class * 12)), 2)

/* Section Header */
#define SH_NAME(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 0),                          4)
#define SH_ADDR(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12),               (class * 4))
#define  SH_OFF(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12 + (class * 4)), (class * 4))
#define SH_SIZE(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12 + (class * 8)), (class * 4))

void sanity_check(unsigned char *buf){
  check_magic(buf);
  class = CLASS(buf);
  if(!((class == ELF32) || (class == ELF64)))
    printf("invalid elf class %d!\n", class);
}

int get_text_address(char *path){
  unsigned char *buf = read_raw(path);
  int text_shd;
  unsigned int shoff, shesz, addr;

  sanity_check(buf);

  /* find the text section */
  if((text_shd = text_section_header(buf)) < 0){
    printf("text section not found\n");
    return -1;
  }

  /* get the size of the text section */
  shoff = SHOFF(buf);
  shesz = SH_E_SZ(buf);
  addr = SH_ADDR(buf,shoff,shesz,text_shd);

  free(buf);
  return addr;
}

int get_text_offset(char *path){
  unsigned char *buf = read_raw(path);
  int text_shd;
  unsigned int shoff, shesz, off;

  sanity_check(buf);

  /* find the text section */
  if((text_shd = text_section_header(buf)) < 0)
    printf("text section not found\n");

  /* get the size of the text section */
  shoff = SHOFF(buf);
  shesz = SH_E_SZ(buf);
  off = SH_OFF(buf,shoff,shesz,text_shd);

  free(buf);
  return off;
}

int get_text_data_size(char *path){
  unsigned char *buf = read_raw(path);
  int text_shd;
  unsigned int shoff, shesz, size;

  sanity_check(buf);

  /* find the text section */
  if((text_shd = text_section_header(buf)) < 0)
    printf("text section not found\n");

  /* get the size of the text section */
  shoff = SHOFF(buf);
  shesz = SH_E_SZ(buf);
  size = SH_SIZE(buf,shoff,shesz,text_shd);

  free(buf);
  return size;
}

unsigned char *get_text_data(char *path){
  unsigned char *buf;
  unsigned char *text;
  int text_shd;
  buf = read_raw(path);

  sanity_check(buf);

  /* find the text section */
  if((text_shd = text_section_header(buf)) < 0)
    printf("text section not found\n");

  /* get the .text data */
  text = section_data(buf, text_shd);

  free(buf);
  return text;
}

unsigned char *section_data(unsigned char *buf, int id){
  int i;
  int shoff = SHOFF(buf);
  int shesz = SH_E_SZ(buf);
  int off = SH_OFF(buf,shoff,shesz,id);
  int size = SH_SIZE(buf,shoff,shesz,id);
  unsigned char *data;

  data = (unsigned char *) malloc(size+1);

  for(i=0;i<size;i++)
    data[i] = buf[off+i];
  return data;
}

int text_section_header(unsigned char *buf){
  int i, j, str_off;
  char name[256];
  int shoff = SHOFF(buf);
  int shesz = SH_E_SZ(buf);
  int shnum = SH_NUM(buf);
  int shsrnd = SH_SRND(buf);
  int* sh_names = malloc(sizeof(int) * shnum);
  int* sh_offsets = malloc(sizeof(int) * shnum);

  /* print the names of the section headers */
  for(i=0;i<shnum;i++){
    sh_names[i] = SH_NAME(buf,shoff,shesz,i);
    sh_offsets[i] = SH_OFF(buf,shoff,shesz,i);
  }

  /* find the .text section */
  str_off = sh_offsets[shsrnd];
  for(i=0;i<shnum;i++){
    j=-1;
    do {
      j++;
      name[j] = buf[str_off + sh_names[i] + j];
    } while (name[j] != 0);
    
    if ((name[0] == '.') &&
        (name[1] == 't') &&
        (name[2] == 'e') &&
        (name[3] == 'x') &&
        (name[4] == 't') &&
        (name[5] == 0))
      return i;
  }

  return -1;
}

int print_header_info(unsigned char *buf){
  sanity_check(buf);

  switch (class){
  case ELF32: printf("class\t32-bit\n"); break;
  case ELF64: printf("class\t64-bit\n"); break;
  }

  /* print out some information about the elf file */
  printf("type\t%d\n",    TYPE(buf));
  printf("machine\t%d\n", MACHINE(buf));
  printf("version\t%d\n", VERSION(buf));
  printf("entry\t%d\n",   ENTRY(buf));
  printf("phoff\t%d\n",   PHOFF(buf));
  printf("shoff\t%d\n",   SHOFF(buf));
  printf("flags\t%d\n",   FLAGS(buf));
  printf("eh_sz\t%d\n",   EH_SZ(buf));
  printf("ph_e_sz\t%d\n", PH_E_SZ(buf));
  printf("ph_num\t%d\n",  PH_NUM(buf));
  printf("sh_e_sz\t%d\n", SH_E_SZ(buf));
  printf("sh_num\t%d\n",  SH_NUM(buf));
  printf("sh_srnd\t%d\n", SH_SRND(buf));
  return 0;
}

unsigned int int_from_bytes(unsigned char *buf, int pos, int num){
  unsigned char * tmp = malloc(sizeof(unsigned char) * num);
  int i;
  unsigned int acc = 0;
  for(i=0;i<num;i++) tmp[i] = buf[(pos + i)];
  for(i=0;i<num;i++) acc += tmp[i] << (8 * i);
  return acc;
}

int check_magic(unsigned char *buf){
  const char *magic = "ELF";
  int i;
  for(i=0; i<4; i++)
    if(!(buf[i] == magic[i])){
      printf("ELF Magic number test failed.");
      return 1;
    }
  return 0;
}

unsigned char *read_raw(char *path){
  FILE *file;
  unsigned char *buf;
  unsigned long length;

  /* open file */
  file = fopen(path, "rb");

  /* find file length */
  fseek(file, 0, SEEK_END);
  length=ftell(file);
  fseek(file, 0, SEEK_SET);

  /* allocate memory */
  buf = malloc(length+1);

  /* read file */
  fread(buf, length, 1, file);
  fclose(file);

  return buf;
}

int file_size(char *path){
  FILE *file;
  unsigned long length;

  file = fopen(path, "rb");
  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fclose(file);

  return length;
}

void write_raw(char *path, unsigned char *buf, int size){
  int i;
  FILE *file;
  file = fopen(path, "w");
  for(i=0;i<size;i++)
    fputc(buf[i], file);
  fclose(file);
}
