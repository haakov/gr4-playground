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

    svr.Options("/block_info", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/block_info", [&registry](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        json data = json::parse(req.body);

        json p_;
        std::string block = data["block"];
        std::println("Block info {}", block);

        if (!registry->contains(block)) {
            p_["error"] = "No such block";
            p_["parameters"] = {};
            p_["inputs"] = {};
            p_["outputs"] = {};
            res.set_content(p_.dump(), "application/json");
            std::println("No such block in registry");
            return;
        }

        // Block exists

        gr::property_map map_;
        auto block_mod = registry->create(block, map_);
        block_mod->settings().init();
        std::vector<std::string> parameters;
        std::vector<std::tuple<std::string,std::string>> inputs, outputs;
        for (const auto& [key, value] : block_mod->settings().defaultParameters()) {
            parameters.push_back(key);
        }
        auto out = block_mod->outputMetaInfos();
        auto in = block_mod->inputMetaInfos();
        for (const auto& item : in) {
            inputs.push_back(std::tuple<std::string,std::string>(item.signal_name.value, item.data_type.value));
        }
        for (const auto& item : out) {
            outputs.push_back(std::tuple<std::string,std::string>(item.signal_name.value, item.data_type.value));
        }

        p_["parameters"] = parameters;
        p_["inputs"] = inputs;
        p_["outputs"] = outputs;

        res.set_content(p_.dump(), "application/json");
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
