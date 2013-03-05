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

unsigned char *buf;
int size, text_offset, text_size = 0;

int text_ind(int offset){
  if(offset >= text_size){
    fprintf(stderr, "Invalid index into text section `%d'.\n", offset);
    abort(); }
  return (offset+text_offset); }

int main(int argc, char *argv[]){
  int c;
  char *in;
  char *help =
    "elf-mutate ELF [ACTIONS..]\n"
    "load ELF format FILE and perform ACTIONS\n"
    "\n"
    "Actions:\n"
    "  -h          print this help information\n"
    "  -n          print the number of bytes\n"
    /* Higher level methods
    "  -d ID       replace instruction ID with nops\n"
    "  -i ID1 ID2  copy instruction ID2 before instruction ID1\n"
    "  -s ID1 ID2  swap instructions ID1 and ID2\n"
    */
    "  -G ID       get the value of byte ID\n"
    "  -S ID NUM   set the value of byte ID to NUM\n"
    "  -o OUTFILE  write current contents to OUTFILE";
  opterr = 0;
  in = NULL;
  buf = NULL;
  size = text_offset = text_size = 0;

  if(argc < 2){ puts(help); abort(); }
  if(argv[1][0] == '-' && argv[1][1] == 'h'){ puts(help); return 1; }
  in = argv[1];

  if(access(in, F_OK) == -1){
    printf("file `%s' not readable\n", in);
    return 1; }

  while ((c = getopt (argc, argv, "hns:o:G:S:")) != -1)
    switch(c){
    case 'n': printf("text size %d\n", get_text_data_size(in)); break;
      /*  Higher level methods
    case 'd':
      GETBUF;
      stmt1 = atoi(optarg);
      delete(buf, stmt1); break;
    case 'i':
      GETBUF;
      stmt1 = atoi(optarg);
      optind++;
      stmt2 = atoi(argv[optind]);
      insert(buf, stmt1, stmt2); break;
    case 's':
      GETBUF;
      stmt1 = atoi(optarg);
      optind++;
      stmt2 = atoi(argv[optind]);
      swap(buf, stmt1, stmt2); break;
      */
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
