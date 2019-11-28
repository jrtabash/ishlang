CPP=g++
CFLAGS=-std=c++17 -fPIC -Wall -Wextra -Wformat -Werror
LFLAGS=
INCS=-I../libishlang
LIBSPATH=-L../build
LIBS=-lishlang
RM=rm -f
CP=cp
BUILD=../build
TARGET=ishlang_unit_test

OBJS=\
	unit_test_main.o \
	unit_test.o \

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

$(TARGET): $(OBJS)
	$(CPP) $(LFLAGS) $(LIBSPATH) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS)) $(LIBS)

unit_test.o: unit_test.cpp unit_test.h
	$(CPP) $(CFLAGS) $(INCS) -c unit_test.cpp -o $(BUILD)/unit_test.o

unit_test_main.o: unit_test_main.cpp unit_test.h
	$(CPP) $(CFLAGS) $(INCS) -c unit_test_main.cpp -o $(BUILD)/unit_test_main.o

clean:
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
