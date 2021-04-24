#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <string>
#include "common.hpp"
#include "crow_all.hpp"

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

typedef struct Sample {
    std::string name;
    double lat;
    double lng;
} Sample;

static void load_samples(Sample samples[], std::ifstream &llfile) {
    std::string line;
    std::vector<std::string> fields;

    for (int i = 0; i < 20; i += 1) {
        std::getline(llfile, line);
        boost::split(fields, line, boost::is_any_of(" "));
        Sample s = {
            .name = fields[0],
            .lat = std::stod(fields[1]),
            .lng = std::stod(fields[2]),
        };
        samples[i] = s;
    }
}

static bool inbounds(const crow::json::rvalue &bounds, double lat, double lng) {
    double north = bounds["north"].d();
    double south = bounds["south"].d();
    double east = bounds["east"].d();
    double west = bounds["west"].d();
    return south < lat && lat < north && west < lng && lng < east;
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

    if (!llfname.length()) {
        std::cerr << "Error: must specify file of sample lat/longs." << std::endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream llfile(llfname);
    if (llfile.fail()) {
        std::cerr << "Error: failed to open latitude/longitude file." << std::endl;
        return EXIT_FAILURE;
    }

    Sample samples[20];
    load_samples(samples, llfile);

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
        auto bounds = crow::json::load(req.body);
        CROW_LOG_INFO << bounds;
        for (auto &x : samples) {
            if (inbounds(bounds, x.lat, x.lng)) {
                CROW_LOG_INFO << x.name;
            }
        }
        return "";
    });

    app.port(8080).run();
    llfile.close();
    return EXIT_SUCCESS;
}
