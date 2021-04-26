#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <string>
#include "common.hpp"
#include "crow_all.hpp"
#include "quadtree.hpp"

#define OPTIONS "hc:i:"
#define BLOCK   4096

Timer timer;

void usage(char *exec) {
    std::cout << "SYNOPSIS\n"
                 "   Geo-genetic visualizer for UShER.\n"
                 "\n"
                 "USAGE\n";
    std::cout << "   " << exec << " [-h] [-i infile]\n";
    std::cout << "\n"
                 "OPTIONS\n"
                 "   -h             Print program usage and help.\n"
                 "   -c llfile      File of lat/longs for samples."
                 "   -i infile      Input MAT protobuf file."
              << std::endl;
}

static void load_samples(QuadTree &qt, const std::unique_ptr<std::istream> &llfile) {
    std::string line;
    std::vector<std::string> fields;

    while (std::getline(*llfile, line)) {
        boost::split(fields, line, boost::is_any_of(" "));
        qt.insert(fields[0], std::stod(fields[1]), std::stod(fields[2]));
    }
}

int main(int argc, char **argv) {
    int opt = 0;
    std::string infname;
    std::string llfname;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'c':
            llfname = optarg;
            break;
        case 'i':
            infname = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // TODO: Read in protobuf, build, tree, extract samples from it.
    // if (!infile.length()) {
    //     std::cerr << "Error: must specify input protobuf to load." << std::endl;
    //     usage(argv[0]);
    //     return EXIT_FAILURE;
    // }

    std::unique_ptr<std::istream> llfile;
    if (llfname.length() > 0) {
        llfile.reset(new std::ifstream(llfname));
    } else {
        llfile.reset(&std::cin);
    }

    QuadTree qt(85.0, -85.0, 180.0, -180.0);
    load_samples(qt, llfile);

    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Info);

    // Home route to display HTML page.
    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        ctx["key"] = std::getenv("GMAPSKEY");
        return crow::mustache::load("index.html").render(ctx);
    });

    // Route for Google Maps API to send bounds to.
    CROW_ROUTE(app, "/data").methods("POST"_method)([&](const crow::request &req) {
        crow::json::rvalue bounds = crow::json::load(req.body);
        CROW_LOG_INFO << bounds;

        crow::json::wvalue res;
        double n = bounds["north"].d();
        double s = bounds["south"].d();
        double e = bounds["east"].d();
        double w = bounds["west"].d();

        for (auto &match : qt.query_range(n, s, e, w)) {
            res[match->sample]["lat"] = match->lat;
            res[match->sample]["lng"] = match->lng;
        }

        return crow::response(res);
    });

    app.port(8080).run();
    return EXIT_SUCCESS;
}
