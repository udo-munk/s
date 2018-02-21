# clang
CC = cc
CFLAGS = -O2 -Wall

# gcc
#CC = gcc
#CFLAGS = -O2 -Wall

s:	s.o address.o adjust.o Bman.o buffer.o commands.o \
	keyboard.o libc.o operator.o screen.o Sman.o yank.o
	$(CC) -o s <$*
