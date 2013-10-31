PROG=dcpu16
NOMAN=1
SRCS=main.c cpu.c
CC=clang
CFLAGS+= -Wall -Wextra -ansi -pedantic

.include <bsd.prog.mk>
