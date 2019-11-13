BUILD=build

all:
	mkdir -p $(BUILD)
	make -C interpreter
	make -C test_runner

clean:
	make clean -C interpreter
	make clean -C test_runner
	rmdir $(BUILD)

install:
	make install -C interpreter
	make install -C test_runner
