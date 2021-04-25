# Geoviz: Geo-genetic visualizer for UShER

See [UShER
documentation](https://usher-wiki.readthedocs.io/en/latest/index.html).

### Requirements

All requirements can be installed via `brew install` or your package manager of
choice.
- `protobuf`
- `boost`
- `tbb`
- `gperftools`

### Build
    $ make

### Clean
    $ make clean

### Notes
- `std::random_shuffle` removed in C++17
    - Fix: swapped with std::shuffle() from `<random>`
- A timer is defined in main but used throughout the library

### TODO
- [ ] Maybe refactor extract main()
- [ ] Set up quadtrees ~~or set up sorted coordinate files~~
- [x] Python Google maps vs. straight Javascript
- [ ] Portability and distribution
