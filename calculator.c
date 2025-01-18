#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"

static const char *skip_space(const char *s) {
  while (isspace(*s)) {
    ++s;
  }
  return s;
}

static int get_precedence(const char *op) {
  switch (*op) {
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  default:
    return 0;
  }
}

static ssize_t evaluate(const char *expr, double *result, int precedence) {
  ssize_t ret;
  double left;
  double right;
  int cur_precedence;
  char cur_op;
  const char *cur = expr;

  cur = skip_space(cur);

  switch (*cur) {
  case '(':
    ++cur;
    if ((ret = evaluate(cur, &left, 0)) < 0) {
      return ret;
    }
    cur += ret;
    if (*cur != ')') {
      return -1;
    }
    ++cur;
    break;
  case '+':
  case '-':
  case '.':
  case '0' ... '9':
    left = strtod(expr, (char **)&cur);
    break;
  default:
    return -1;
  }

  cur = skip_space(cur);

  while ((cur_precedence = get_precedence(cur)) > precedence) {
    cur_op = *cur++;
    if ((ret = evaluate(cur, &right, cur_precedence)) < 0) {
      return ret;
    }
    cur = skip_space(cur + ret);

    switch (cur_op) {
    case '+':
      left += right;
      break;
    case '-':
      left -= right;
      break;
    case '*':
      left *= right;
      break;
    case '/':
      left /= right;
      break;
    }
  }

  if (isdigit(*cur) || *cur == '(' || *cur == '.') {
    if ((ret = evaluate(cur, &right, precedence)) < 0) {
      return ret;
    }
    cur += ret;
    left *= right;
  }

  *result = left;

  return cur - expr;
}

int main() {
  char *line = NULL;
  double result;
  ssize_t ret;

  rl_bind_key('\t', rl_insert);

  while (true) {
    line = readline("> ");
    if (line == NULL) {
      printf("\n");
      return 0;
    }
    add_history(line);

    ret = evaluate(line, &result, 0);
    if (ret < 0 || line[ret] != '\0') {
      fprintf(stderr, "error evaluating expression\n");
    } else {
      printf("$ %lg\n", result);
    }

    free(line);
  }
}
