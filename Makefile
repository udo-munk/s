# clang
CC = cc
CFLAGS = -O2 -Wall -std=c89

# gcc
#CC = gcc
#CFLAGS = -O2 -Wall -std=c89

s:	s.o address.o adjust.o Bman.o buffer.o commands.o \
	keyboard.o libc.o operator.o screen.o Sman.o yank.o
	$(CC) -o s <$*

install:
	strip s
	cp s $HOME/bin

clean:
	rm -f *.o s
