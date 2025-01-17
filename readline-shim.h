#ifndef CALCULATOR_READLINE_SHIM_H
#define CALCULATOR_READLINE_SHIM_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

static char *readline(const char* prompt) {
  char *line = NULL;
  size_t line_cap = 0;
  ssize_t line_len;
  printf("%s", prompt);
  if ((line_len = getline(&line, &line_cap, stdin)) < 0) {
    return NULL;
  }
  line[line_len - 1] = '\0';
  return line;
}

static void* rl_insert = NULL;
static int rl_bind_key(int key, void* f) { return 0; }

static void add_history(const char *entry) {}

#endif // CALCULATOR_READLINE_SHIM_H
