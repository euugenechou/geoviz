samples = 50000

all: compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

gui: compile
	build/geoviz -i protobufs/test.pb -c latlngs/demo.csv

tree: compile
	build/geoviz -i protobufs/test.pb -o /tmp/treedump.tsv

clean:
	rm -rf build
