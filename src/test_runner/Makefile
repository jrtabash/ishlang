CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f
BIN=../binaries

OBJS=make_test_files.o

make_test_files: $(OBJS)
	$(CPP) -o $(BIN)/make_test_files $(patsubst %, $(BIN)/%, $(OBJS))

make_test_files.o: make_test_files.cpp
	$(CPP) $(CFLAGS) -c make_test_files.cpp -o $(BIN)/make_test_files.o

clean:
	$(RM) $(BIN)/*.o
	$(RM) $(BIN)/make_test_files

install:
	cp  $(BIN)/make_test_files ~/bin/
	cp  run_tests.sh ~/bin/
