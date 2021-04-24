all: default

default:
	@mkdir -p build && cd build && cmake .. && make -j4

clean:
	rm -rf build