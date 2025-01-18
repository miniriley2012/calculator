# Calculator

This project provides a minimal infix calculator REPL. All numbers are represented as C doubles. Numbers may be
inputted in decimal or hex and may have exponents (e.g. `1e4` or `0x5p6`), or whatever form is accepted by `strtod`.
The four arithmetic operations, addition, subtraction, multiplication, and division are supported. Numbers not
seperated by an operator will be multiplied. So `2 3 4` will be interpreted as `2 * 3 * 4`. Numbers do not necessarily
have to be separated by whitespace. For example, `3.4.5` will be interpreted as `3.4 * .5`. Additionally, parentheses
may be used to group operations.

## Building

First, run the configure script. This will attempt to find libedit or readline on your system using pkg-config, and if
it is not found a fallback shim based on POSIX getline will be used.

```sh
./configure
```

Two files, `readline.h`, which includes the readline API, and `options.mk`, which includes the necessary compiler flags
for including and linking libedit or readline, should have been created.

You can now run make to generate the executable.

```sh
make
```

An executable, `calculator`, should be generated. Any arguments passed are ignored. Input is provided through stdin
until EOF.

## License

All files in this repository are licensed as ISC. Though if the library is linked with readline from FSF, then the
final binary will be under GPL.
