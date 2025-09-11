#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define FILENAME "db.txt"

int main(int argc, char** argv) {
  int i, key, result;
  FILE* fd = NULL;
  char command;
  char* command_raw;
  char* data;
  command_raw = (char*) malloc(sizeof(char[2]));
  data        = (char*) malloc(sizeof(char[1024]));

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
    printf("%c: %i=%s\n", command, key, data);

    if (fd == NULL) {
      fd = fopen(FILENAME, "w+");
    }

    if (fd == NULL) {
      printf("couldn't open file '%s': error code %d\n", FILENAME, errno);
      return 1;
    }
  }
}

