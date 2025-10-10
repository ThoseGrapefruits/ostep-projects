#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define DB_FILE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP
#define FILENAME_I "db.indices"
#define FILENAME_D "db.data"
#define FILE_SIZE_STEP 4096
#define LINE_LIM 1023
#define KEY_LEN 32

struct ifile {
  size_t key_count;
  ptrdiff_t index_end;
  int keys[1023];
  ptrdiff_t indices[1023];
};

int main(int argc, char** argv) {
  bool init, found;
  int arg, i, index, key, result, fd_d, fd_i;
  struct stat sb_d, sb_i;
  struct ifile *iptr;
  char* dptr;

  fd_d = open(FILENAME_D, O_RDWR|O_CREAT, DB_FILE_FLAGS);

  if (fd_d == -1) {
    printf("couldn't open file '%s': error code %d\n", FILENAME_D, errno);
    return 1;
  }

  fd_i = open(FILENAME_I, O_RDWR|O_CREAT, DB_FILE_FLAGS);

  if (fd_i == -1) {
    printf("couldn't open file '%s': error code %d\n", FILENAME_I, errno);
    return 1;
  }

  if (fstat(fd_i, &sb_i) == -1) {
    printf("couldn't stat file '%s': error code %d\n", FILENAME_I, errno);
    return 1;
  }

  if (fstat(fd_d, &sb_d) == -1) {
    printf("couldn't stat file '%s': error code %d\n", FILENAME_D, errno);
    return 1;
  }

  init = false;

  if (!sb_i.st_size) {
    init = true;
    ftruncate(fd_i, FILE_SIZE_STEP);
    sb_i.st_size = FILE_SIZE_STEP;
  }

  if (!sb_d.st_size) {
    ftruncate(fd_d, FILE_SIZE_STEP);
    sb_d.st_size = FILE_SIZE_STEP;
  }

  dptr = mmap(
    NULL,
    sb_d.st_size, // open whole file
    PROT_WRITE,
    MAP_SHARED,
    fd_d,
    0
  );

  assert(dptr != (void *) -1);

  iptr = mmap(
    NULL,
    sb_i.st_size, // open whole file
    PROT_WRITE,
    MAP_SHARED,
    fd_i,
    0
  );

  assert(iptr != (void *) -1);

  if (init) {
    printf("INIT\n");
    iptr->key_count = 0;
    iptr->index_end = 0;
    *dptr = '\0';
  }

  char command;
  char *command_raw, *data;
  command_raw = (char*) malloc(sizeof(char[2]));
  data        = (char*) malloc(sizeof(char[LINE_LIM+1]));

  for (arg = 0; arg < argc-1; arg++) {
    command_raw[0] = '\0';
    data[0] = '\0';
    key = 0;
    result = sscanf(argv[arg+1], "%[pgdca],%i,%s", command_raw, &key, data);

    if (result < 1) {
      printf("bad input for argument %d (%d): '%s'\n", arg+1, result, argv[i]);
      return 1;
    }

    command = command_raw[0];

    switch (command) {
      case 'a':
        break; // TODO implement all (list all entries)
      case 'c':
        break; // TODO implement clear (delete all entries)
      case 'd':
        break; // TODO implement delete (delete one entry)
      case 'p': {
        found = false;
        for (i = 0; i < iptr->key_count; i++) {
          if (iptr->keys[i] == key) {
            found = true;
            index = iptr->indices[i];
            printf(
              "%d %ld %ld %ld dsize %s\n",
              index,
              strlen(data),
              strlen(dptr+index),
              iptr->index_end,
              dptr+index
            );

            if (strlen(data) <= strlen(dptr+index)) { // in-place data swap
              if (strcmp(data, dptr+index)) {
                strcpy(dptr+index, data);
                *(dptr+index+strlen(data)+1) = '\0';
                printf("%d=%s (updated, dtswp)\n", key, data);
              }
            } else { // in-place index swap
              // TODO expansion check
              iptr->indices[i] = index = iptr->index_end+1;
              printf("%d %d=%s (updating %d, ixswp)\n", i, key, data, index);
              iptr->index_end = strlen(data)+index-dptr;
              strcpy(dptr+index, data);
              *(dptr+iptr->index_end) = '\0';
            }

            break;
          }
        }

        if (!found) {
          // TODO expansion check
          i = iptr->key_count++;
          iptr->keys[i] = key;
          iptr->indices[i] = index = iptr->index_end+1;
          iptr->index_end = iptr->indices[i] + strlen(data);
          strcpy(dptr+iptr->indices[i], data);
          *(dptr+iptr->index_end) = '\0';
          printf("%d=%s (%d inserted @ %ld, %s)\n",
              key, data, index, iptr->indices[i], dptr+iptr->indices[i]);
        }

        break;
      }
      case 'g': {
        found = false;
        for (i = 0; i < iptr->key_count; i++)
          if (iptr->keys[i] == key) {
            index = iptr->indices[i];
            data = dptr+index;
            printf("%d=%s (i: %d, index: %d)\n", key, data, i, index);
            found = true;
            break;
          }

        if (!found) {
          printf("No data found for key %d\n", key);
        }

        break;
      }
    }
  }

  fsync(fd_d);
  fsync(fd_i);

  for (i = 0; i < iptr->key_count; i++) {
    printf("indices: %d %ld %s\n", iptr->keys[i], iptr->indices[i], dptr + iptr->indices[i]);
  }
}

