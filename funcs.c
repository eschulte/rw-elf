#include <errno.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <stdio.h>
#include <sysexits.h>

int main(int argc, char *argv[]){
  Elf *e;

  if(start_elf() != 0)
    printf("start_elf() failed\n");

  if(read_elf(argv[1], e) != 0)
    printf("read_elf() failed\n");

  printf(".text section ID is %d\n", text_id(e));
}

int start_elf(){
  if(elf_version(EV_CURRENT) == EV_NONE)
    errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));
  return 0;
}

int read_elf(char *path, Elf *e){
  int fd = -1;

  if((fd=open (path, O_RDWR, 0)) < 0)
    err(EX_NOINPUT, " open \"% s \" failed ", path);

  if((e=elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

  return 0;
}

int text_id(Elf *e){
  int n = 0;
  char *name;
  Elf_Scn *scn = NULL;
  GElf_Shdr shdr;
  size_t shstrndx;

  /* find the .text section */
  if(elf_getshdrstrndx(e, &shstrndx) != 0)
    errx(EX_SOFTWARE, " elf_getshdrstrndx () failed : %s.", elf_errmsg(-1));

  while((scn=elf_nextscn(e, scn)) != NULL) {
    if(gelf_getshdr(scn, &shdr) != &shdr)
      errx(EX_SOFTWARE, "getshdr() failed: %s.", elf_errmsg(-1));
    if((name=elf_strptr(e, shstrndx, shdr.sh_name)) == NULL)
      errx(EX_SOFTWARE, "elf_strptr() failed: %s. ", elf_errmsg(-1));
    n++;
    if((strcmp(".text", name) == 0)) break;
  }
  if(scn == NULL){
    printf(".text section not found.\n");
    return -1;
  }

  return n;
}
