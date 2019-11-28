CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
CP=cp
BUILD=../build
TARGET=make_test_files

OBJS=make_test_files.o

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

$(TARGET): $(OBJS)
	$(CPP) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS))

make_test_files.o: make_test_files.cpp
	$(CPP) $(CFLAGS) -c make_test_files.cpp -o $(BUILD)/make_test_files.o

clean:
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
	$(CP) run_tests.sh ~/bin/
