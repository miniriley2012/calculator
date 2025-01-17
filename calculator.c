#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"

enum operator{
  OPERATOR_INVALID,
  OPERATOR_ADD,
  OPERATOR_SUBTRACT,
  OPERATOR_MULTIPLY,
  OPERATOR_DIVIDE,
  OPERATOR_GROUP,
};

enum precedence {
  PRECEDENCE_INVALID,
  PRECEDENCE_ADDITION,
  PRECEDENCE_MULTIPLICATION,
  PRECEDENCE_GROUP,
};

static enum operator get_operator(const char *s, char **end) {
  enum operator op;

  switch (*s) {
  case '+':
    op = OPERATOR_ADD;
    break;
  case '-':
    op = OPERATOR_SUBTRACT;
    break;
  case '*':
    op = OPERATOR_MULTIPLY;
    break;
  case '/':
    op = OPERATOR_DIVIDE;
    break;
  case '(':
    op = OPERATOR_GROUP;
    break;
  case '0' ... '9':
    if (end != NULL) {
      *end = (char *)s;
    }
    return OPERATOR_MULTIPLY;
  default:
    op = OPERATOR_INVALID;
    return op;
  }

  if (end != NULL) {
    *end = (char *)s + 1;
  }

  return op;
}

static enum precedence get_precedence(enum operator op) {
  static const enum precedence operator_precedence[] = {
      PRECEDENCE_INVALID,        PRECEDENCE_ADDITION,       PRECEDENCE_ADDITION,
      PRECEDENCE_MULTIPLICATION, PRECEDENCE_MULTIPLICATION, PRECEDENCE_GROUP,
  };
  return operator_precedence[op];
}

static double get_constant(const char *s, char **end) {
  static const char *constants[] = {
      "e", "pi", "nan", "inf", "infinity",
  };
  static const double values[] = {
      2.71828182845904523536028747135266250,
      3.14159265358979323846264338327950288,
  };
  int i = 0;
  size_t s_len;
  const char *begin = s;
  bool found = false;

  while (isalpha(*s)) {
    ++s;
  }

  s_len = s - begin;

  for (; i < 5; ++i) {
    if (strlen(constants[i]) != s_len) {
      continue;
    }

    for (size_t j = 0; j < s_len; ++j) {
      if (tolower(begin[j]) != tolower(constants[i][j])) {
        goto outer;
      }
    }

    found = true;
    break;

  outer:;
  }

  if (!found) {
    if (end != NULL) {
      *end = (char *)s;
    }
    return NAN;
  } else if (i < 2) {
    if (end != NULL) {
      *end = (char *)s;
    }
    return values[i];
  } else {
    return strtod(begin, end);
  }
}

static char *skip_space(const char *s) {
  while (isspace(*s)) {
    ++s;
  }
  return (char *)s;
}

static int evaluate(const char *expr, char **expr_end, double *result,
                    int precedence) {
  double left;
  double right;
  char *end;
  enum operator op;
  enum precedence cur_precedence;
  int err;

  expr = skip_space(expr);

  switch (*expr) {
  case '(':
    if (expr[1] == '\0') {
      return -1;
    }
    err = evaluate(expr + 1, &end, &left, 0);
    if (err < 0) {
      return err;
    }
    if (*end != ')') {
      return -1;
    }
    ++end;
    break;
  case ')':
    return -1;
  case 'A' ... 'Z':
  case 'a' ... 'z':
    left = get_constant(expr, &end);
    break;
  case '+':
  case '-':
  case '.':
  case '0' ... '9':
    left = strtod(expr, &end);
    break;
  default:
    return -1;
  }

  expr = skip_space(end);

  op = get_operator(expr, &end);

  cur_precedence = get_precedence(op);

  while (cur_precedence > precedence) {
    err = evaluate(expr, &end, &right, cur_precedence);
    if (err < 0) {
      return err;
    }
    expr = skip_space(end);

    switch (op) {
    case OPERATOR_ADD:
      left += right;
      break;
    case OPERATOR_SUBTRACT:
      left -= right;
      break;
    case OPERATOR_GROUP:
    case OPERATOR_MULTIPLY:
      left *= right;
      break;
    case OPERATOR_DIVIDE:
      left /= right;
      break;
    default:
      return -1;
    }

    op = get_operator(expr, &end);
    expr = skip_space(end);
    cur_precedence = get_precedence(op);
  }

  *result = left;

  if (expr_end != NULL) {
    *expr_end = (char *)expr;
  }

  return 0;
}

int main() {
  char *line = NULL;
  char *line_end;
  double result;
  int err;

  rl_bind_key('\t', rl_insert);

  while (true) {
    line = readline("> ");
    if (line == NULL) {
      printf("\n");
      break;
    }
    add_history(line);

    err = evaluate(line, &line_end, &result, 0);
    if (err < 0 || *line_end != '\0') {
      fprintf(stderr, "error evaluating expression\n");
      continue;
    }

    printf("$ %lg\n", result);

    free(line);
  }
}
