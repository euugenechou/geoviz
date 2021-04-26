all: compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

test: compile
	./randsamples.py 50000 | build/geoviz

clean:
	rm -rf build
