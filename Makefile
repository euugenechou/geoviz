samples = 50000

all: compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

gui: compile
	build/geoviz -i protobufs/test.pb -o /tmp/treedump.tsv -c latlngs/samples_22600.csv -s samples/test.samples

tree: compile
	build/geoviz -i protobufs/test.pb -o /tmp/treedump.tsv

samples: compile
	build/geoviz -i protobufs/test.pb -o /tmp/treedump.tsv -s samples/test.samples

clean:
	rm -rf build
