all: run

run: compile
	build/geoviz

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

clean:
	rm -rf build
