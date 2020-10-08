CPP=g++
CFLAGS=-std=c++17 -fPIC -Wall -Wextra -Wformat -Werror
LFLAGS=-shared
RM=rm -f
CP=cp
BUILD=../build
TARGET=libishlang.so

OBJS=\
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
	module.o

ifeq ($(DEBUG), 0)
	CFLAGS += -DNDEBUG -O2
else
	CFLAGS += -DDEBUG
endif

$(TARGET): $(OBJS)
	$(CPP) $(LFLAGS) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS))

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

module.o: module.cpp module.h environment.h parser.h
	$(CPP) $(CFLAGS) -c module.cpp -o $(BUILD)/module.o

clean:
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
