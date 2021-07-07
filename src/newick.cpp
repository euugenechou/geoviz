#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include "common.hpp"
#include "filter.hpp"
#include "summary.hpp"

#define OPTIONS "hi:o:"

Timer timer;

static void usage(char *exec) {
    std::cout << "SYNOPSIS\n"
                 "   Sample dumper for UShER protobufs.\n"
                 "\n"
                 "USAGE\n";
    std::cout << "   " << exec << " [-h] [-i infile] [-o outfile]\n";
    std::cout << "\n"
                 "OPTIONS\n"
                 "   -h             Print program usage and help.\n"
                 "   -i infile      Input MAT protobuf file. (default: stdin)\n"
                 "   -o outfile     Output for samples. (default: stdout)"
              << std::endl;
}

int main(int argc, char **argv) {
    int opt = 0;
    std::string infname, outfname;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infname = optarg;
            break;
        case 'o':
            outfname = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Setup default input.
    std::shared_ptr<std::istream> infile(&std::cin, [](...) {});
    if (infname.length() > 0) {
        infile.reset(new std::ifstream(infname));
    }

    // Setup default output.
    std::shared_ptr<std::ostream> outfile(&std::cout, [](...) {});
    if (outfname.length() > 0) {
        outfile.reset(new std::ofstream(outfname));
    }

    // Load MAT from infile, write to outfile.
    MAT::Tree tree = MAT::from_newick_input(*infile);
    assert(tree.root);

    return EXIT_SUCCESS;
}
