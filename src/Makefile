BUILD=build
MKDIR=mkdir -p
RMDIR=rmdir
DBGFLAG=1

ifeq ($(DEBUG), 0)
	DBGFLAG=0
endif

all:
	$(MKDIR) $(BUILD)
	make DEBUG=$(DBGFLAG) -C libishlang
	make DEBUG=$(DBGFLAG) -C interpreter
	make DEBUG=$(DBGFLAG) -C test_runner

clean:
	make clean -C libishlang
	make clean -C interpreter
	make clean -C test_runner
	$(RMDIR) $(BUILD)

install:
	make install -C libishlang
	make install -C interpreter
	make install -C test_runner
