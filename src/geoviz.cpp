#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "common.hpp"
#include "crow_all.hpp"
#include "filter.hpp"
#include "quadtree.hpp"
#include "summary.hpp"

#define OPTIONS "hc:i:o:"

Timer timer;

static void usage(char *exec) {
    std::cout << "SYNOPSIS\n"
                 "   Geo-genetic visualizer for UShER.\n"
                 "\n"
                 "USAGE\n";
    std::cout << "   " << exec << " [-h] [-i infile]\n";
    std::cout << "\n"
                 "OPTIONS\n"
                 "   -h             Print program usage and help.\n"
                 "   -c llfile      File of lat/longs for samples.\n"
                 "   -i infile      Input MAT protobuf file.\n"
                 "   -o outfile     Output of samples in input protobuf.\n"
                 "   -s sfile       File of samples."
              << std::endl;
}

static void load_samples(QuadTree &qt, std::ifstream &llfile) {
    std::string line;
    std::vector<std::string> fields;

    while (std::getline(llfile, line)) {
        boost::split(fields, line, boost::is_any_of(" "));
        assert(qt.insert(fields[0], std::stod(fields[1]), std::stod(fields[2])));
    }
}

int main(int argc, char **argv) {
    int opt = 0;
    std::string infname, outfname, llfname, sfname;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'c':
            llfname = optarg;
            break;
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

    // Load MAT from input file.
    MAT::Tree tree = MAT::load_mutation_annotated_tree(infname);
    if (!tree.root) {
        return EXIT_FAILURE;
    }

    // Open lat/lng file.
    std::ifstream llfile(llfname);
    if (!llfile) {
        std::cerr << "Error: failed to open " << llfname << std::endl;
        return EXIT_FAILURE;
    }

    // Init worldwide QuadTree then load it with samples in lat/lng file.
    QuadTree qt(85.0, -85.0, 180.0, -180.0);
    load_samples(qt, llfile);

    // Start micro-webserver.
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

        std::vector<std::string> samples;

        for (auto &match : qt.query(n, s, e, w)) {
            samples.push_back(match->sample);
            res[match->sample]["lat"] = match->lat;
            res[match->sample]["lng"] = match->lng;
        }

        MAT::Tree subtree = get_sample_subtree(tree, samples);

        if (subtree.root) {
            res["newick"] = get_newick_string(subtree, false, false, false, false);
        } else {
            res["newick"] = "";
        }

        return crow::response(res);
    });

    app.port(8080).run();
    return EXIT_SUCCESS;
}
