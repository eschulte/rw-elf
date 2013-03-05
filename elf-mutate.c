#include "elf.h"

int main(int argc, char *argv[]){
  char *in, *out, *buf;
  long int size;
  int offset, text_size;

  if(argc != 3){
    printf("usage: elf [input-file] [output-file]\n");
    return 1;
  }

  in        = argv[1];
  out       = argv[2];
  buf       = read_raw(in);
  size      = file_size(in);
  offset    = get_text_offset(in);
  text_size = get_text_data_size(in);

  /* update the last byte of the .text section */
  buf[(offset + text_size - 1)] = (char)190;

  write_raw(out, buf, size);

  return 0;
}
