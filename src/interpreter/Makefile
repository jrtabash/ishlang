CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
CP=cp
BUILD=../build

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

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

ishlang: $(OBJS)
	$(CPP) -o $(BUILD)/ishlang $(patsubst %, $(BUILD)/%, $(OBJS))

main.o: main.cpp util.h value.h exception.h environment.h lambda.h instance.h sequence.h code_node.h unit_test.h interpreter.h
	$(CPP) $(CFLAGS) -c main.cpp -o $(BUILD)/main.o

exception.o: exception.h exception.cpp
	$(CPP) $(CFLAGS) -c exception.cpp -o $(BUILD)/exception.o

util.o: util.h util.cpp exception.h
	$(CPP) $(CFLAGS) -c util.cpp -o $(BUILD)/util.o

value.o: value.h value.cpp lambda.h instance.h sequence.h exception.h
	$(CPP) $(CFLAGS) -c value.cpp -o $(BUILD)/value.o

environment.o: environment.cpp environment.h value.h exception.h
	$(CPP) $(CFLAGS) -c environment.cpp -o $(BUILD)/environment.o

lambda.o: lambda.cpp lambda.h value.h environment.h code_node.h exception.h
	$(CPP) $(CFLAGS) -c lambda.cpp -o $(BUILD)/lambda.o

struct.o: struct.cpp struct.h
	$(CPP) $(CFLAGS) -c struct.cpp -o $(BUILD)/struct.o

instance.o: instance.cpp instance.h value.h exception.h
	$(CPP) $(CFLAGS) -c instance.cpp -o $(BUILD)/instance.o

sequence.o: sequence.cpp sequence.h value.h exception.h
	$(CPP) $(CFLAGS) -c sequence.cpp -o $(BUILD)/sequence.o

code_node.o: code_node.cpp code_node.h value.h parser.h environment.h lambda.h util.h exception.h
	$(CPP) $(CFLAGS) -c code_node.cpp -o $(BUILD)/code_node.o

lexer.o: lexer.cpp lexer.h util.h exception.h
	$(CPP) $(CFLAGS) -c lexer.cpp -o $(BUILD)/lexer.o

parser.o: parser.cpp parser.h lexer.h code_node.h util.h exception.h
	$(CPP) $(CFLAGS) -c parser.cpp -o $(BUILD)/parser.o

unit_test.o: unit_test.cpp unit_test.h value.h environment.h lambda.h instance.h sequence.h util.h exception.h
	$(CPP) $(CFLAGS) -c unit_test.cpp -o $(BUILD)/unit_test.o

interpreter.o: interpreter.cpp interpreter.h environment.h parser.h exception.h
	$(CPP) $(CFLAGS) -c interpreter.cpp -o $(BUILD)/interpreter.o

clean: 
	$(RM) $(BUILD)/*.o
	$(RM) $(BUILD)/ishlang

install:
	$(CP) $(BUILD)/ishlang ~/bin/
