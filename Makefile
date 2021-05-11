all: doc compile

compile:
	@mkdir -p build && cd build && cmake .. && make -j4

gui: doc compile
	build/geoviz -i protobufs/test.pb -c latlngs/demo.csv

tree: doc compile
	build/geoviz -i protobufs/test.pb -o /tmp/treedump.tsv

doc:
	doxygen docs/Doxyfile

clean:
	rm -rf build docs/html
