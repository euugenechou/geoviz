#include <cstdlib>
#include <iostream>
#include "common.hpp"
#include "crow_all.hpp"

Timer timer;

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        ctx["key"] = std::getenv("GMAPSKEY");
        auto page = crow::mustache::load("index.html");
        return page.render(ctx);
    });

    CROW_ROUTE(app, "/data").methods("POST"_method)
    ([](const crow::request& req) {
        auto bounds = crow::json::load(req.body);
        std::cout << "North: " << bounds["north"] << std::endl;
        std::cout << "South: " << bounds["south"] << std::endl;
        std::cout << "East: " << bounds["east"] << std::endl;
        std::cout << "West: " << bounds["west"] << std::endl;
        return "";
    });

    app.port(8080).multithreaded().run();
}
