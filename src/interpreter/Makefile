CPP=g++
CFLAGS=-std=c++17 -fPIC -Wall -Wextra -Wformat -Werror
LFLAGS=
INCS=-I../libishlang
LIBSPATH=-L../build
LIBS=-lishlang
RM=rm -f
CP=cp
BUILD=../build
TARGET=ishlang

OBJS=\
	main.o \
	unit_test.o \
	interpreter.o

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

$(TARGET): $(OBJS)
	$(CPP) $(LFLAGS) $(LIBSPATH) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS)) $(LIBS)

main.o: main.cpp unit_test.h interpreter.h
	$(CPP) $(CFLAGS) $(INCS) -c main.cpp -o $(BUILD)/main.o

unit_test.o: unit_test.cpp unit_test.h
	$(CPP) $(CFLAGS) $(INCS) -c unit_test.cpp -o $(BUILD)/unit_test.o

interpreter.o: interpreter.cpp interpreter.h
	$(CPP) $(CFLAGS) $(INCS) -c interpreter.cpp -o $(BUILD)/interpreter.o

clean: 
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
