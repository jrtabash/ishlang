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
	ishlang_main.o \
	interpreter.o

ifeq ($(DEBUG), 0)
	CFLAGS += -DNDEBUG -O2
else
	CFLAGS += -DDEBUG
endif

$(TARGET): $(OBJS)
	$(CPP) $(LFLAGS) $(LIBSPATH) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS)) $(LIBS)

ishlang_main.o: ishlang_main.cpp interpreter.h
	$(CPP) $(CFLAGS) $(INCS) -c ishlang_main.cpp -o $(BUILD)/ishlang_main.o

interpreter.o: interpreter.cpp interpreter.h
	$(CPP) $(CFLAGS) $(INCS) -c interpreter.cpp -o $(BUILD)/interpreter.o

clean: 
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
