#include "elf.h"

int main(int argc, char *argv[]){
  if(argc != 3){
    printf("usage: elf [input-file] [output-file]\n");
    return 1;
  }

  char *in      = argv[1];
  char *out     = argv[2];
  char *buf     = read_raw(in);
  long int size = file_size(in);
  int offset    = get_text_offset(in);
  int text_size = get_text_data_size(in);
  
  /* update the last byte of the .text section */
  buf[(offset + text_size - 1)] = 190;
  
  write_raw(out, buf, size);

  return 0;
}
