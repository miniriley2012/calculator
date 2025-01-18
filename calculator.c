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

static const struct operator{
  char operator;
  int precedence;
  size_t len;
}
operators[] = {
    {'+', 1, 1}, {'-', 1, 1}, {'*', 2, 1}, {'/', 2, 1}, {'(', 3, 0},
};

static struct operator get_operator(const char *op) {
  switch (*op) {
  case '+':
    return operators[0];
  case '-':
    return operators[1];
  case '*':
    return operators[2];
  case '/':
    return operators[3];
  case '(':
    return operators[4];
  default:
    return (struct operator){ 0, 0 };
  }
}

static ssize_t evaluate(const char *expr, double *result, int precedence) {
  ssize_t ret;
  double left;
  double right;
  struct operator op;
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

  while ((op = get_operator(cur)).precedence > precedence) {
    cur += op.len;
    if ((ret = evaluate(cur, &right, op.precedence)) < 0) {
      return ret;
    }
    cur = skip_space(cur + ret);

    switch (op.operator) {
    case '+':
      left += right;
      break;
    case '-':
      left -= right;
      break;
    case '(':
    case '*':
      left *= right;
      break;
    case '/':
      left /= right;
      break;
    }
  }

  if (isdigit(*cur)) {
    if ((ret = (evaluate(cur, &right, precedence)) < 0)) {
      return ret;
    }
    expr += ret;
    left *= right;
  }

  *result = left;

  return cur - expr;
}

int main() {
  char *line = NULL;
  double result;
  int err;

  rl_bind_key('\t', rl_insert);

  while (true) {
    line = readline("> ");
    if (line == NULL) {
      printf("\n");
      return 0;
    }
    add_history(line);

    err = evaluate(line, &result, 0);
    if (err < 0) {
      fprintf(stderr, "error evaluating expression\n");
    } else {
      printf("$ %lg\n", result);
    }

    free(line);
  }
}
