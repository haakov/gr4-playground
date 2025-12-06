#include <iostream>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>
#include <csignal>

#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/Scheduler.hpp>

#include <GrBasicBlocks.hpp>

#include <gnuradio-4.0/basic/ConverterBlocks.hpp>
#include <gnuradio-4.0/basic/SignalGenerator.hpp>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

void enableCORS(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void signal_handler(int signum) {
    std::cout << "\n\n SIGINT received.\n";
    exit(signum);
}

int main() {
    signal(SIGINT, signal_handler);  // Catch SIGINT
    // TODO: Remove when GR gets proper blocks library
    auto* registry = grGlobalBlockRegistry();
    auto block_count = registry->keys().size();
    std::vector<std::string> blocks;
    for (auto key : registry->keys()) {
        std::string tmp = key;
        blocks.push_back(tmp);
    }

    json j;
    j["blocks"] = blocks;

    gr::blocklib::initGrBasicBlocks(*registry);

    std::println("Blocks in registry: {}", block_count);

    httplib::Server svr;

    svr.Options("/block_count", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/block_count", [&](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
    	std::println("Block count");
        res.set_content(std::format("{{\"result\": {}}}", block_count), "application/json");
    });

    svr.Options("/blocks", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/blocks", [&](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
    	std::println("Blocks");
        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
