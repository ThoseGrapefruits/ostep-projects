#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define DB_FILE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP
#define FILENAME_I "db.indices"
#define FILENAME_D "db.data"
#define INIT_FILE_SIZE 4096
#define LINE_LIM 1023
#define KEY_LEN 32

struct ifile {
  int icount;
  int iend;
  char indices[1023];
};

int main(int argc, char** argv) {
  bool init = false;
  int i, key, result, foundline;
  int fd_i = open(FILENAME_I, O_RDWR|O_CREAT, DB_FILE_FLAGS);
  int fd_d = open(FILENAME_D, O_RDWR|O_CREAT, DB_FILE_FLAGS);
  struct stat sb_d, sb_i;
  struct ifile *iptr;
  char* dptr;

  if (fd_d == -1) {
    printf("couldn't open file '%s': error code %d\n", FILENAME_D, errno);
    return 1;
  }

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

  if (!sb_i.st_size) {
    init = true;
    ftruncate(fd_i, INIT_FILE_SIZE);
    sb_i.st_size = INIT_FILE_SIZE;
  }

  if (!sb_d.st_size) {
    ftruncate(fd_d, INIT_FILE_SIZE);
    sb_d.st_size = INIT_FILE_SIZE;
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
    iptr->icount = 0;
    iptr->iend   = 0;
  }

  char command;
  char *command_raw, *data;
  command_raw = (char*) malloc(sizeof(char[2]));
  data        = (char*) malloc(sizeof(char[LINE_LIM+1]));

  for (i = 0; i < argc-1; i++) {
    command_raw[0] = '\0';
    data[0] = '\0';
    key = 0;
    result = sscanf(argv[i+1], "%[pgdca],%i,%s", command_raw, &key, data);

    if (result < 1) {
      printf("bad input for argument %d (%d): '%s'\n", i+1, result, argv[i]);
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
        foundline = -1;
        // while ((result = fscanf(fd, "%i=%s\n", &linekey, linedata)) > 0) {
          // if (linekey == key) {
            // foundline = line;
            // break;
          // }
          // line++;
        // }

        // if (foundline < 0) {
          // fd = freopen(FILENAME, "a", fd);
          // fprintf(fd, "%d=%s\n", key, data);
        // } else {
          // TODO implement replacement put
        // }

        break;
      }
      case 'g': {
        foundline = -1;
        // while ((result = fscanf(fd, "%i=%s\n", &linekey, linedata)) > 0) {
          // if (linekey == key) {
            // foundline = line;
            // printf("%d=%s\n", linekey, linedata);
            // break;
          // }
          // line++;
        // }

        if (foundline < 0)
          printf("%d: key does not exist\n", key);

        break;
      }
    }
  }
}

