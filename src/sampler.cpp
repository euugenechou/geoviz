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
    std::cout << "   " << exec << " [-h] [-i infile]\n";
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
    std::unique_ptr<std::istream> infile;
    if (infname.length() > 0) {
        infile.reset(new std::ifstream(infname));
    } else {
        infile.reset(&std::cin);
    }

    // Setup default output.
    std::unique_ptr<std::ostream> outfile;
    if (outfname.length() > 0) {
        outfile.reset(new std::ofstream(outfname));
    } else {
        outfile.reset(&std::cout);
    }

    // Load MAT from infile, write to outfile.
    MAT::Tree tree = MAT::from_input(*infile);
    dump_samples(tree, *outfile);

    return EXIT_SUCCESS;
}
