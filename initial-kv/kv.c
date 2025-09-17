#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FILENAME "db.txt"
#define LINE_LIM 1023
#define KEY_LEN 32

int main(int argc, char** argv) {
  int i, line, key, result, linekey, foundline;
  int fd = open(FILENAME, O_RDWR);
  struct stat sb;

  if (fd == -1) {
    printf("couldn't open file '%s': error code %d\n", FILENAME, errno);
    return 1;
  }

  if (fstat(fd, &sb) == -1) {
    printf("couldn't stat file '%s': error code %d\n", FILENAME, errno);
    return 1;
  }

  void *fptr = mmap(
    NULL,
    sb.st_size, // open whole file
    PROT_WRITE,
    MAP_SHARED,
    fd,
    0
  );

  assert(fptr != (void *) -1);

  char command;
  char *command_raw, *data, *linedata;
  command_raw = (char*) malloc(sizeof(char[2]));
  data        = (char*) malloc(sizeof(char[LINE_LIM+1]));
  linedata    = (char*) malloc(sizeof(char[LINE_LIM-KEY_LEN+1]));

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
        line = 0;
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
        line = 0;
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

