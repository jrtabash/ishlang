CPP=g++
CFLAGS=-std=c++17 -Wall -Wextra -Wformat -Werror
RM=rm -f

OBJS=make_test_files.o

make_test_files: $(OBJS)
	$(CPP) -o make_test_files $(OBJS)

make_test_files.o: make_test_files.cpp
	$(CPP) $(CFLAGS) -c make_test_files.cpp

clean:
	$(RM) *.o
	$(RM) make_test_files
