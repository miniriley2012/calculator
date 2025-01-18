.POSIX:
.SUFFIXES:

include config.mk

CFLAGS+=-std=c99

calculator: calculator.c readline.h readline-shim.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o calculator calculator.c
