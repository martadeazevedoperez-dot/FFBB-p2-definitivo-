
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

EXE = menu
OBJ = menu.o ../odbc-examples/odbc.o
HEADERS = ../odbc-examples/odbc.h

all: compile

compile: $(EXE)

%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -I../odbc-examples -c -o $@ $<

../odbc-examples/odbc.o: ../odbc-examples/odbc.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -I../odbc-examples -c -o $@ $<

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LDLIBS)

clean:
	rm -f *.o ../odbc-examples/odbc.o core $(EXE)
