BUILD=build
MKDIR=mkdir -p
RMDIR=rmdir
DBGFLAG=1
CPPSTD=c++20

ifeq ($(DEBUG), 0)
	DBGFLAG=0
endif

all:
	$(MKDIR) $(BUILD)
	make DEBUG=$(DBGFLAG) CPPSTD=$(CPPSTD) -C libishlang
	make DEBUG=$(DBGFLAG) CPPSTD=$(CPPSTD) -C interpreter
	make DEBUG=$(DBGFLAG) CPPSTD=$(CPPSTD) -C unit_test
	make DEBUG=$(DBGFLAG) CPPSTD=$(CPPSTD) -C test_runner

clean:
	make clean -C libishlang
	make clean -C interpreter
	make clean -C unit_test
	make clean -C test_runner
	$(RMDIR) $(BUILD)

install:
	make install -C libishlang
	make install -C interpreter
	make install -C unit_test
	make install -C test_runner
