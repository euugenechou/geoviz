#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <string>
#include "common.hpp"
#include "crow_all.hpp"
#include "filter.hpp"
#include "quadtree.hpp"
#include "summary.hpp"

#define OPTIONS "hc:i:o:s:"

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
                 "   -o outfile     Output of samples in input protobuf."
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

static std::vector<std::string> load_sample_names(std::ifstream &sfile) {
    std::string line;
    std::vector<std::string> samples;

    while (std::getline(sfile, line)) {
        samples.push_back(line);
    }

    return samples;
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
        case 's':
            sfname = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Open sample file.
    std::ifstream sfile(sfname);
    if (!sfile) {
        std::cerr << "Error: failed to open " << sfname << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string> samples = load_sample_names(sfile);
    for (auto &s : samples) {
        std::cout << s << std::endl;
    }

    // Load MAT from input file.
    MAT::Tree tree = MAT::load_mutation_annotated_tree(infname);
    if (!tree.root) {
        return EXIT_FAILURE;
    }

    MAT::Tree subtree = get_sample_subtree(tree, samples);
    std::cout << get_newick_string(subtree, true, false, false, false) << std::endl;

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

        for (auto &match : qt.query(n, s, e, w)) {
            res[match->sample]["lat"] = match->lat;
            res[match->sample]["lng"] = match->lng;
        }

        res["newick"] = "(" + std::to_string(rand() % 100) + ")";

        return crow::response(res);
    });

    app.port(8080).run();
    return EXIT_SUCCESS;
}
