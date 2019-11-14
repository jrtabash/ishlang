CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
CP=cp
BUILD=../build

OBJS=make_test_files.o

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

make_test_files: $(OBJS)
	$(CPP) -o $(BUILD)/make_test_files $(patsubst %, $(BUILD)/%, $(OBJS))

make_test_files.o: make_test_files.cpp
	$(CPP) $(CFLAGS) -c make_test_files.cpp -o $(BUILD)/make_test_files.o

clean:
	$(RM) $(BUILD)/*.o
	$(RM) $(BUILD)/make_test_files

install:
	$(CP) $(BUILD)/make_test_files ~/bin/
	$(CP) run_tests.sh ~/bin/
