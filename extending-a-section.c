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
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>

static char *base_data = "hello world";
static char *extra_data = "goodbye world";

int main(int argc, char *argv[])
{
  int error, fd, result;
  Elf *e;
  Elf_Scn *scn;
  Elf_Data *d;
  off_t fsz1, fsz2;
  char *buf;

  e = NULL;
  fd = -1;

  /* sanity checks */
  if ( argc != 2)
    errx(EX_USAGE, " usage : % s file - name ", argv [0]);
  if ( elf_version ( EV_CURRENT ) == EV_NONE )
    errx(EX_SOFTWARE, " ELF library initialization failed : % s ",
         elf_errmsg(-1));

  if((fd=open (argv[1], O_RDWR, 0)) < 0)
    err(EX_NOINPUT, " open \"% s \" failed ", argv[1]);

  if((e=elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

  /* Retrieve section 1 and extend it. */
  if ((scn = elf_getscn(e, 13)) == NULL) /* <- 13 indexes the .text section */
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
  if ((error = elf_end(e)) != 0)
    printf("elf_end() failed: \"%s\".\n", elf_errmsg(-1));

  (void) close(fd);
  return 0;
}
