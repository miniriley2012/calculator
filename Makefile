.POSIX:
.SUFFIXES:

include config.mk

calculator: calculator.c readline.h readline-shim.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o calculator calculator.c
