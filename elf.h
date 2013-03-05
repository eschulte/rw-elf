#include <stdio.h>
#include <stdlib.h>

void             sanity_check(char *buf);
unsigned char *      read_raw(char *path);
int                 file_size(char *path);
void                write_raw(char *path, unsigned char *buf, int size);
int               check_magic(char *buf);
int                 elf_class(char *buf);
unsigned int   int_from_bytes(char *buf, int pos, int num);
int         print_header_info(char *buf);
int       text_section_header(char *buf);
unsigned char *  section_data(char *buf, int id);
int          get_text_address(char *path);
int           get_text_offset(char *path);
int        get_text_data_size(char *path);
unsigned char * get_text_data(char *path);
