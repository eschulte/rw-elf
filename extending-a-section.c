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
  unsigned int flag;
  struct stat sb;
  Elf *e;
  Elf_Scn *scn;
  Elf_Data *d;
  const char *srcfile = argv[1];
  const char *reffile = argv[2];
  off_t fsz1, fsz2;
  char *tfn;

  e = NULL;
  fd = -1;
  tfn = NULL;

  /* sanity checks */
  if ( argc != 2)
    errx(EX_USAGE, " usage : % s file - name ", argv [0]);
  if ( elf_version ( EV_CURRENT ) == EV_NONE )
    errx(EX_SOFTWARE, " ELF library initialization failed : % s ",
         elf_errmsg(-1));

  /* /\* Make a copy of the reference object. *\/ */
  /* if ((tfn = elfts_copy_file(srcfile, &error)) < 0) { */
  /*   printf("elfts_copyfile(%s) failed: \"%s\".", srcfile, strerror(error)); */
  /* } */

  if((fd=open (argv[1], O_RDWR, 0)) < 0)
    err(EX_NOINPUT, " open \"% s \" failed ", argv[1]);

  if((e=elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

  if (stat(reffile, &sb) < 0) {
    printf("fstat() failed: \"%s\".\n", strerror(errno));
  }

  /* Retrieve section 1 and extend it. */
  if ((scn = elf_getscn(e, 1)) == NULL) {
    printf("elf_getscn() failed: \"%s\".\n", elf_errmsg(-1));
  }

  if ((d = elf_newdata(scn)) == NULL) {
    printf("elf_newdata() failed: \"%s\".\n", elf_errmsg(-1));
  }

  d->d_buf = extra_data;
  d->d_size = strlen(extra_data);

  if (elf_flagscn(scn, ELF_C_SET, ELF_F_DIRTY) != ELF_F_DIRTY) {
    printf("elf_flagscn() failed: \"%s\".\n", elf_errmsg(-1));
  }

  if ((fsz1 = elf_update(e, ELF_C_NULL)) < 0) {
    printf("elf_update(NULL) failed: \"%s\".\n", elf_errmsg(-1));
  }

  if (fsz1 != sb.st_size) {
    printf("Size error: expected=%ld, elf_update()=%ld\n", sb.st_size, fsz1);
  }

  if ((fsz2 = elf_update(e, ELF_C_WRITE)) < 0) {
    printf("elf_update(WRITE) failed: \"%s\".\n", elf_errmsg(-1));
  }

  if (fsz1 != fsz2) {
    printf("fsz1 (%ld) != fsz2 (%ld)", fsz1, fsz2);
  }

  /* Close the temporary file. */
  if ((error = elf_end(e)) != 0) {
    printf("elf_end() failed: \"%s\".\n", elf_errmsg(-1));
  }

  (void) close(fd);
  return 0;
}
