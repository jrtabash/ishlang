CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
CP=cp
BUILD=../build
TARGET=ishlang_test_files

OBJS=ishlang_test_files.o

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

$(TARGET): $(OBJS)
	$(CPP) -o $(BUILD)/$(TARGET) $(patsubst %, $(BUILD)/%, $(OBJS))

ishlang_test_files.o: ishlang_test_files.cpp
	$(CPP) $(CFLAGS) -c ishlang_test_files.cpp -o $(BUILD)/ishlang_test_files.o

clean:
	$(RM) $(patsubst %, $(BUILD)/%, $(OBJS))
	$(RM) $(BUILD)/$(TARGET)

install:
	$(CP) $(BUILD)/$(TARGET) ~/bin/
	$(CP) ishlang_run_tests.sh ~/bin/
