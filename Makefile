# clang on OSX
#CC = clang
#CFLAGS = -O2 -Wall -Wextra -std=c89

# clang or gcc on Linux
#CC = gcc
CC = clang
CFLAGS = -O2 -Wall -std=c89 -DTERMIOS

# Mark Williams C on COHERENT
#CC = cc
#CFLAGS = -O

s:	s.o address.o adjust.o Bman.o buffer.o commands.o \
	keyboard.o lib.o operator.o screen.o Sman.o yank.o
	$(CC) -o s *.o

install:
	strip s
	cp s ${HOME}/bin

clean:
	rm -f *.o s
