#include <stdio.h>
#include <stdlib.h>

void             sanity_check(unsigned char *buf);
unsigned char *      read_raw(char *path);
int                 file_size(char *path);
void                write_raw(char *path, unsigned char *buf, int size);
int               check_magic(unsigned char *buf);
int                 elf_class(unsigned char *buf);
unsigned int   int_from_bytes(unsigned char *buf, int pos, int num);
int         print_header_info(unsigned char *buf);
int       text_section_header(unsigned char *buf);
unsigned char *  section_data(unsigned char *buf, int id);
int          get_text_address(char *path);
int           get_text_offset(char *path);
int        get_text_data_size(char *path);
unsigned char * get_text_data(char *path);
