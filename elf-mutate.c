#include "elf.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void number(char *in){
  printf("text size %d\n", get_text_data_size(in)); }

void delete(char *buf, int stmt1){
  buf[stmt1] = 0; }

void insert(char *buf, int stmt1, int stmt2){
  buf[stmt1+1] = buf[stmt2]; }

void swap(char *buf, int stmt1, int stmt2){
  char tmp=buf[stmt1];
  buf[stmt1] = buf[stmt2];
  buf[stmt2] = tmp; }

int main(int argc, char *argv[]){
  int c, stmt1, stmt2;
  char *in, *buf;
  char *help =
    "elf-mutate ELF [ACTIONS..]\n"
    "load ELF format FILE and perform ACTIONS\n"
    "\n"
    "Actions:\n"
    "  -h          print this help information\n"
    "  -n          print the number of bytes\n"
    "  -d ID       replace instruction ID with nops\n"
    "  -i ID1 ID2  copy instruction ID2 before instruction ID1\n"
    "  -s ID1 ID2  swap instructions ID1 and ID2\n"
    "  -o OUTFILE  write current elf contents to OUTFILE";
  opterr = 0;
  buf = NULL;

  if(argc < 2){ puts(help); abort(); }
  if(argv[1][0] == '-' && argv[1][1] == 'h'){ puts(help); return 1; }
  in = argv[1];

  if(access(in, F_OK) == -1){
    printf("file `%s' not readable\n", in);
    return 1; }

  while ((c = getopt (argc, argv, "hnd:i:s:o:")) != -1)
    switch(c){
    case 'n': number(in); break;
    case 'd':
      stmt1 = atoi(optarg);
      optind++;
      stmt2 = atoi(argv[optind]);
      if(! buf){ buf = read_raw(in); }
      delete(buf, stmt1); break;
    case 'i':
      stmt1 = atoi(optarg);
      optind++;
      stmt2 = atoi(argv[optind]);
      insert(buf, stmt1, stmt2); break;
    case 's':
      stmt1 = atoi(optarg);
      optind++;
      stmt2 = atoi(argv[optind]);
      swap(buf, stmt1, stmt2); break;
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
