#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define INITIAL_STACK 1024

int main(int argc, char **argv) {
  if (argc > 3) {
    fprintf(stderr, "usage: reverse <input> <output>\n");
    return 1;
  }

  if (argc == 3 && !strcmp(argv[1], argv[2])) {
    fprintf(stderr, "reverse: input and output file must differ\n");
    return 1;
  }

  FILE *fd_in =  argc < 2 ? stdin : fopen(argv[1], "r");

  if (fd_in == NULL) {
    fprintf(stderr, "reverse: cannot open file '%s'\n", argc < 2 ? "stdin" : argv[1]);
    return 1;
  }

  FILE *fd_out = argc < 3 ? stdout : fopen(argv[2], "w+");

  if (fd_out == NULL) {
    fprintf(stderr, "reverse: cannot open file '%s'\n", argc < 3 ? "stdout" : argv[2]);
    return 1;
  }

  struct stat fs_in;
  fstat(fileno(fd_in), &fs_in);
  struct stat fs_out;
  fstat(fileno(fd_out), &fs_out);

  if (fs_in.st_dev == fs_out.st_dev && fs_in.st_ino == fs_out.st_ino) {
    fprintf(stderr, "reverse: input and output file must differ\n");
    return 1;
  }

  ssize_t line_result;
  size_t line_size;
  int stack_size = INITIAL_STACK;
  int stack_index = 0;
  char** stack = malloc(stack_size * sizeof(char*));

  while ((line_result = getline(&stack[stack_index++], &line_size, fd_in)) >= 0) {
    if (stack_index == stack_size) {
      stack_size *= 2;
      stack = reallocarray(stack, stack_size, sizeof(char*));
    }
  }

  while (stack_index-- > 0) {
    fprintf(fd_out, stack[stack_index]);
  }

  return 0;
}

