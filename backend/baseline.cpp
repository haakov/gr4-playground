
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>


#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/Scheduler.hpp>

#include <GrBasicBlocks.hpp>
#include <GrElectricalBlocks.hpp>
#include <GrFileIoBlocks.hpp>
#include <GrFilterBlocks.hpp>
#include <GrFourierBlocks.hpp>
#include <GrHttpBlocks.hpp>
#include <GrMathBlocks.hpp>
#include <GrTestingBlocks.hpp>
#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/Scheduler.hpp>
#include <gnuradio-4.0/basic/ConverterBlocks.hpp>
#include <gnuradio-4.0/basic/SignalGenerator.hpp>

#include "httplib.h"
#include <iostream>

void enableCORS(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    // TODO: Remove when GR gets proper blocks library
    auto* registry = grGlobalBlockRegistry();
    auto block_count = registry->keys().size();

    gr::blocklib::initGrBasicBlocks(*registry);
    gr::blocklib::initGrElectricalBlocks(*registry);
    gr::blocklib::initGrFileIoBlocks(*registry);
    gr::blocklib::initGrFilterBlocks(*registry);
    gr::blocklib::initGrFourierBlocks(*registry);
    gr::blocklib::initGrHttpBlocks(*registry);
    gr::blocklib::initGrMathBlocks(*registry);
    gr::blocklib::initGrTestingBlocks(*registry);

    std::println("Blocks in registry: {}", block_count);

    httplib::Server svr;

    // 1. Handle the Preflight (OPTIONS) request
    // The browser asks: "Can I send a POST with JSON?"
    svr.Options("/blocks/count", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204; // No Content (Successful generic response)
    });


    svr.Post("/blocks/count", [&](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res); // MUST add CORS headers here too!

        std::cout << "Received blocks" << std::endl;

        // Your logic here
        res.set_content(std::format("{{\"result\": {}}}", block_count), "application/json");
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);


    return 0;
}
