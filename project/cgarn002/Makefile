# Makefile 
# Guoyong Shi, 2014-2015

SRC=\
	Symbol_Table.c \
	parse_func.c \
	MNA_Matrix.c \
	main.c 

CC = g++

CFLAGS = -I ./eigen/ -g -c

OBJ=$(SRC:.c=.o)
OBJS= main.o MNA_Matrix.o parse_func.o Symbol_Table.o

.c.o:
	g++ -I ./Eigen/ -g -c $(SRC)

all: parse runparse

parse: parse.lex parse.y
	flex -oparse.yy.c parse.lex
	bison -d parse.y

main.o: main.c MNA_Matrix.h parse_func.h Symbol_Table.h
	$(CC) $(CFLAGS) main.c

MNA_Matrix.o: MNA_Matrix.h MNA_Matrix.c parse_func.h Symbol_Table.h
	$(CC) $(CFLAGS) MNA_Matrix.c

parse_func.o: parse_func.h parse_func.c Symbol_Table.h
	$(CC) $(CFLAGS) parse_func.c

Symbol_Table.o: Symbol_Table.h Symbol_Table.c
	$(CC) $(CFLAGS) Symbol_Table.c

runparse: $(OBJS)
	g++ -I ./eigen/ -g -o $@ $(OBJS)

clean:
	rm -f *.o *.obj *.exe parse.yy.c parse.tab.h parse.tab.c

