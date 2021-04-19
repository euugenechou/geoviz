#include "logging.hpp"
#include <iostream>

bool logging = false;

void _info(std::string msg) {
    if (logging) {
        std::cerr << "[info]\t" << msg << std::endl;
    }
}

void _err(std::string msg) {
    if (logging) {
        std::cerr << "[error]\t" << msg << std::endl;
    }
}

void _debug(std::string file, size_t ln, std::string fn, std::string msg) {
    if (logging) {
        std::cerr << "[debug]\t";
        std::cerr << file << ":" << ln << ":" << fn << "()\t";
        std::cerr << msg << std::endl;
    }
}
