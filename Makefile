all: compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

test: compile
	build/geoviz -c latlngs/test.csv

clean:
	rm -rf build
