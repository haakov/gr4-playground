#include <iostream>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>
#include <csignal>

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
    auto blocks = registry->keys();

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

    svr.Options("/blocks/count", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/blocks/count", [&](const httplib::Request& req, httplib::Response& res) {
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
        res.set_content(std::format("{{\"result\": {}}}", block_count), "application/json"); # TODO FIXME
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
