/*
 * Test extending a section.
 * from the tests for
 * https://sourceforge.net/apps/trac/elftoolchain
 */
#include <errno.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <stdio.h>
#include <sysexits.h>

int main(int argc, char *argv[])
{
  int fd, n;
  Elf *e;
  char *buf, *name;
  Elf_Scn *scn;
  Elf_Data *d;
  GElf_Shdr shdr;
  size_t shstrndx;

  e = NULL;
  scn = NULL;
  fd = -1;
  n = 0;

  /* sanity checks */
  if (argc != 2)
    errx(EX_USAGE, " usage : % s file - name ", argv [0]);
  if (elf_version(EV_CURRENT) == EV_NONE)
    errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));

  if((fd=open (argv[1], O_RDWR, 0)) < 0)
    err(EX_NOINPUT, " open \"% s \" failed ", argv[1]);

  if((e=elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

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
    return 1;
  }

  /* Retrieve the .text section and edit it. */
  if ((scn = elf_getscn(e, n)) == NULL)
    printf("elf_getscn() failed: \"%s\".\n", elf_errmsg(-1));

  if ((d = elf_getdata(scn, NULL)) == NULL)
    printf("getting data failed: \"%s\".\n", elf_errmsg(-1));

  buf = (char *) d->d_buf;
  printf("Old value of .text[471] was %x.\n", ((char *) d->d_buf)[471]);

  if ((d = elf_newdata(scn)) == NULL)
    printf("elf_newdata() failed: \"%s\".\n", elf_errmsg(-1));

  /* <- change the last byte from nop to ret -- this has no runtime effect*/
  d->d_buf = buf;
  ((char *) d->d_buf)[471] = 195;

  printf("New value of .text[471] is  %x.\n", ((char *) d->d_buf)[471]);

  /* flag the section as dirty */
  if (elf_flagscn(scn, ELF_C_SET, ELF_F_DIRTY) != ELF_F_DIRTY)
    printf("elf_flagscn() failed: \"%s\".\n", elf_errmsg(-1));

  /* Sync the section */
  if (elf_update(e, ELF_C_WRITE) < 0)
    printf("elf_update() failed: \"%s\".", elf_errmsg(-1));

  /* Close the temporary file. */
  if (elf_end(e) != 0)
    printf("elf_end() failed: \"%s\".\n", elf_errmsg(-1));

  (void) close(fd);
  return 0;
}
