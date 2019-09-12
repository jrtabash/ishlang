CPP=g++ -std=c++17
RM=rm -f

OBJS=\
	main.o \
	exception.o \
	util.o \
	value.o \
	environment.o \
	lambda.o \
	struct.o \
	instance.o \
	byte_code.o \
	lexer.o \
	parser.o \
	unit_test.o \
	interpreter.o

ishlang: $(OBJS)
	$(CPP) -o ishlang $(OBJS)

main.o: main.cpp util.h value.h exception.h environment.h lambda.h byte_code.h unit_test.h interpreter.h
	$(CPP) -c main.cpp

exception.o: exception.h exception.cpp
	$(CPP) -c exception.cpp

util.o: util.h util.cpp exception.h
	$(CPP) -c util.cpp

value.o: value.h value.cpp lambda.h exception.h
	$(CPP) -c value.cpp

environment.o: environment.cpp environment.h value.h exception.h
	$(CPP) -c environment.cpp

lambda.o: lambda.cpp lambda.h value.h environment.h byte_code.h exception.h
	$(CPP) -c lambda.cpp

struct.o: struct.cpp struct.h
	$(CPP) -c struct.cpp

instance.o: instance.cpp instance.h value.h exception.h
	$(CPP) -c instance.cpp

byte_code.o: byte_code.cpp byte_code.h value.h parser.h environment.h lambda.h util.h exception.h
	$(CPP) -c byte_code.cpp

lexer.o: lexer.cpp lexer.h util.h exception.h
	$(CPP) -c lexer.cpp

parser.o: parser.cpp parser.h lexer.h byte_code.h util.h exception.h
	$(CPP) -c parser.cpp

unit_test.o: unit_test.cpp unit_test.h value.h environment.h lambda.h util.h exception.h
	$(CPP) -c unit_test.cpp

interpreter.o: interpreter.cpp interpreter.h environment.h parser.h exception.h
	$(CPP) -c interpreter.cpp

clean: 
	$(RM) *.o
	$(RM) ishlang

install:
	cp ishlang ~/bin/
