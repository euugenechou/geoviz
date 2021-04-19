# Genevieve

Hm.

### Requirements

All requirements can be installed via `brew install` or your package manager of
choice.
 - `protobuf`
 - `boost`
 - `tbb`

### Notes
 - `std::random_shuffle` removed in C++17
  - Fix: swapped with std::shuffle() from `<random>`
 - A timer is defined in main but used throughout the library

### Build
  $ make

### Clean
  $ make clean
