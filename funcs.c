#include <errno.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <stdio.h>
#include <sysexits.h>

void start_elf();
Elf *read_elf(char *path);
int text_id(Elf *e);
char *section_data(Elf *e, int id);
void update_section_data(Elf *e, int id, char *buf);
void write_elf(Elf *e);

int main(int argc, char *argv[]){
  Elf *e;
  int id;
  char *buf;

  start_elf();

  if((e=read_elf(argv[1])) == NULL)
    printf("read_elf() failed\n");

  if((id=text_id(e)) < 0)
    printf("text_id() failed\n");

  buf=section_data(e, id);

  /* change the last value off the .text data */
  buf[471] = 195;

  update_section_data(e, id, buf);

  write_elf(e);
}

void start_elf(){
  if(elf_version(EV_CURRENT) == EV_NONE)
    errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));
}

Elf *read_elf(char *path){
  Elf *e;
  int fd = -1;

  if((fd=open(path, O_RDWR, 0)) < 0)
    err(EX_NOINPUT, "open \"%s\" failed", path);

  if((e=elf_begin(fd, ELF_C_RDWR, NULL)) == NULL)
    errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));

  return (e);
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

char *section_data(Elf *e, int id){
  Elf_Scn *scn = NULL;
  Elf_Data *d;

  /* Retrieve the .text section and edit it. */
  if ((scn = elf_getscn(e, id)) == NULL)
    printf("elf_getscn() failed: \"%s\".\n", elf_errmsg(-1));

  if ((d = elf_getdata(scn, NULL)) == NULL)
    printf("getting data failed: \"%s\".\n", elf_errmsg(-1));

  /* <- change the last byte from nop to ret -- this has no runtime effect*/
  return (char *) d->d_buf;
}

void update_section_data(Elf *e, int id, char *buf){
  Elf_Data *d;
  Elf_Scn *scn = NULL;

  /* Retrieve the .text section and edit it. */
  if ((scn = elf_getscn(e, id)) == NULL)
    printf("elf_getscn() failed: \"%s\".\n", elf_errmsg(-1));

  /* Get the new section for updating */
  if ((d = elf_newdata(scn)) == NULL)
    printf("elf_newdata() failed: \"%s\".\n", elf_errmsg(-1));

  d->d_buf = buf;

  /* flag the section as dirty */
  if (elf_flagscn(scn, ELF_C_SET, ELF_F_DIRTY) != ELF_F_DIRTY)
    printf("elf_flagscn() failed: \"%s\".\n", elf_errmsg(-1));
}

void write_elf(Elf *e){
  /* attempting to change the file to which the elf file is written */
  /* this doesn't work because e_fd is private (defined in _Elf) */
  /* if((e->e_fd=open(path, O_WRONLY, 0)) < 0) */
  /*   err(EX_NOINPUT, " open \"% s \" failed ", path); */

  /* Sync the section */
  if (elf_update(e, ELF_C_WRITE) < 0)
    printf("elf_update() failed: \"%s\".", elf_errmsg(-1));

  /* Close the temporary file. */
  if (elf_end(e) != 0)
    printf("elf_end() failed: \"%s\".\n", elf_errmsg(-1));
}
