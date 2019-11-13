CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
BIN=../binaries

OBJS=\
	main.o \
	exception.o \
	util.o \
	value.o \
	environment.o \
	lambda.o \
	struct.o \
	instance.o \
	sequence.o \
	code_node.o \
	lexer.o \
	parser.o \
	unit_test.o \
	interpreter.o

ishlang: $(OBJS)
	$(CPP) -o $(BIN)/ishlang $(patsubst %, $(BIN)/%, $(OBJS))

main.o: main.cpp util.h value.h exception.h environment.h lambda.h instance.h sequence.h code_node.h unit_test.h interpreter.h
	$(CPP) $(CFLAGS) -c main.cpp -o $(BIN)/main.o

exception.o: exception.h exception.cpp
	$(CPP) $(CFLAGS) -c exception.cpp -o $(BIN)/exception.o

util.o: util.h util.cpp exception.h
	$(CPP) $(CFLAGS) -c util.cpp -o $(BIN)/util.o

value.o: value.h value.cpp lambda.h instance.h sequence.h exception.h
	$(CPP) $(CFLAGS) -c value.cpp -o $(BIN)/value.o

environment.o: environment.cpp environment.h value.h exception.h
	$(CPP) $(CFLAGS) -c environment.cpp -o $(BIN)/environment.o

lambda.o: lambda.cpp lambda.h value.h environment.h code_node.h exception.h
	$(CPP) $(CFLAGS) -c lambda.cpp -o $(BIN)/lambda.o

struct.o: struct.cpp struct.h
	$(CPP) $(CFLAGS) -c struct.cpp -o $(BIN)/struct.o

instance.o: instance.cpp instance.h value.h exception.h
	$(CPP) $(CFLAGS) -c instance.cpp -o $(BIN)/instance.o

sequence.o: sequence.cpp sequence.h value.h exception.h
	$(CPP) $(CFLAGS) -c sequence.cpp -o $(BIN)/sequence.o

code_node.o: code_node.cpp code_node.h value.h parser.h environment.h lambda.h util.h exception.h
	$(CPP) $(CFLAGS) -c code_node.cpp -o $(BIN)/code_node.o

lexer.o: lexer.cpp lexer.h util.h exception.h
	$(CPP) $(CFLAGS) -c lexer.cpp -o $(BIN)/lexer.o

parser.o: parser.cpp parser.h lexer.h code_node.h util.h exception.h
	$(CPP) $(CFLAGS) -c parser.cpp -o $(BIN)/parser.o

unit_test.o: unit_test.cpp unit_test.h value.h environment.h lambda.h instance.h sequence.h util.h exception.h
	$(CPP) $(CFLAGS) -c unit_test.cpp -o $(BIN)/unit_test.o

interpreter.o: interpreter.cpp interpreter.h environment.h parser.h exception.h
	$(CPP) $(CFLAGS) -c interpreter.cpp -o $(BIN)/interpreter.o

clean: 
	$(RM) $(BIN)/*.o
	$(RM) $(BIN)/ishlang

install:
	cp $(BIN)/ishlang ~/bin/
