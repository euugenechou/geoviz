samples = 50000

all: compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

test: compile
	./randsamples.py $(samples) | build/geoviz

clean:
	rm -rf build
