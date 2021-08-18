CC = g++
CFLAGS = -Wall -std=c++14
DEPS = headers.h functions.h
OBJ = globalvariables.o opendirectory.o navigate.o commandmode.o deletecommand.o createcommand.o renamecommand.o gotocommand.o copycommand.o  movecommand.o searchcommand.o main.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

tfe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *o