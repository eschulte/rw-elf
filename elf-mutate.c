#include "elf.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define CHECKIN {if(!in){fprintf(stderr, "Infile required.\n"); abort();}}
#define GETBUF  {CHECKIN; if(!buf){             \
      buf=read_raw(in);                         \
      size=file_size(in);                       \
      text_offset=get_text_offset(in);          \
      text_size=get_text_data_size(in); } }

char *in = NULL;
unsigned char *buf = NULL;
int size, text_offset, text_size = 0;

int text_ind(int offset){
  if(offset >= text_size){
    fprintf(stderr, "Invalid index into text section `%d'.\n", offset);
    abort(); }
  return (offset+text_offset); }

int main(int argc, char *argv[]){
  char *help =
    "elf-mutate ELF [ACTIONS..]\n"
    "load ELF format FILE and perform ACTIONS\n"
    "\n"
    "Actions:\n"
    "  -h          print this help information\n"
    "  -n          print the number of .text bytes\n"
    "  -G ID       get the value of .text byte ID\n"
    "  -S ID NUM   set the value of .text byte ID to NUM\n"
    "  -o OUTFILE  write current contents to OUTFILE";
  int c;
  opterr = 0;

  if(argc < 2 || (argv[1][0] == '-' && argv[1][1] == 'h'))
    puts(help); return 1;
  in = argv[1];

  if(access(in, F_OK) == -1){
    printf("File `%s' is not readable.\n", in); abort(); }

  while ((c = getopt (argc, argv, "hns:o:G:S:")) != -1)
    switch(c){
    case 'n': printf("text size %d\n", get_text_data_size(in)); break;
    case 'G':
      GETBUF;
      printf("%x\n", buf[text_ind(atoi(optarg))]); break;
    case 'S':
      GETBUF;
      buf[text_ind(atoi(optarg))] = atoi(argv[optind]); break;
    case 'o':
      write_raw(optarg, buf, size);
      chmod(optarg, strtol("0777", 0, 8));
      break;
    case 'h': puts(help); return 1;
    case '?':
      if (optopt == '1' || optopt == '2')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      abort();
    default: abort();
    }

  return 0;
}
