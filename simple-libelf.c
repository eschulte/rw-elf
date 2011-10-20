/* Modify an existing ELF object. */
#include <err.h>
#include <fcntl.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int fd;
  Elf *e;
  Elf64_Ehdr *ehdr;

  // print usage information
  if(argc != 2)
    errx(EX_USAGE, "usage : %s file-name", argv [0]);

  // set the elf version
  if(elf_version(EV_CURRENT) == EV_NONE)
    errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));

  // open the file
  if((fd = open(argv [1], O_RDWR, 0777)) < 0)
    err(EX_OSERR, "open \%s \" failed", argv [1]);

  // create the elf object
  if((e = elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

  // read the elf header
  if((ehdr = elf64_getehdr(e)) == NULL)
    errx(EX_SOFTWARE, "elf64_getehdr() failed: %s.", elf_errmsg(-1));

  // compute the layout of the object
  if(elf_update(e, ELF_C_NULL) < 0)
    errx(EX_SOFTWARE, "elf_update(NULL) failed: %s.", elf_errmsg(-1));

  // write out the resulting elf object
  if(elf_update(e, ELF_C_WRITE) < 0)
    errx(EX_SOFTWARE, "elf_update() failed: %s.", elf_errmsg(-1));
  
  // close up shop
  (void) elf_end(e);
  (void) close(fd);
  exit(EX_OK);
}
