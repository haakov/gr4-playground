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
    httplib::Server svr;

    signal(SIGINT, signal_handler);  // Catch SIGINT
    // TODO: Remove when GR gets proper blocks library
    auto* registry = grGlobalBlockRegistry();
    gr::blocklib::initGrBasicBlocks(*registry);

    svr.Options("/block_count", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/block_count", [&registry](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
    	std::println("Block count");
        auto block_count = registry->keys().size();
        res.set_content(std::format("{{\"result\": {}}}", block_count), "application/json");
    });

    svr.Options("/blocks", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/blocks", [&registry](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        std::vector<std::map<std::string,std::string>> blocks;
        json j;

        for (auto key : registry->keys()) {
            auto block = std::map<std::string,std::string>();
            block["id"] = key;
            block["label"] = key;
            block["category"] = "Basic";
            block["inputs"] = "Basic";
            blocks.push_back(block);
        }
        j["blocks"] = blocks;
        j["blocksByCategory"] = std::map<std::string,std::vector<std::string>>();
        j["blocksByCategory"]["Basic"] = blocks;
        j["categories"] = std::vector<std::string>({"Basic"});
        j["total_blocks"] = blocks.size();
        j["generated_at"] = std::string("10:00 01.01.2026");
    	std::println("Blocks");

        res.set_content(j.dump(), "application/json");
    });

    svr.Options("/block_info", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/block_info", [&registry](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        json j;
        std::vector<std::tuple<std::string,std::string>> parameters;
        std::vector<std::tuple<std::string,std::string>> inputs, outputs;

        json data = json::parse(req.body);
        std::string block = data["block"];
        std::println("Block info {}", block);

        if (!registry->contains(block)) {
            j["error"] = "No such block";
            j["parameters"] = {};
            j["inputs"] = {};
            j["outputs"] = {};
            res.set_content(j.dump(), "application/json");
            std::println("No such block in registry");
            return;
        }

        // Block exists
        gr::property_map map_;
        auto block_mod = registry->create(block, map_);
        block_mod->settings().init();

        for (const auto& [key, value] : block_mod->settings().defaultParameters()) {
            std::string val_s = std::visit(gr::meta::overloaded{
                          [&](double val) { return std::to_string(val); },
                          [&](float val) { return std::to_string(val); },
                          [&](auto&& val) {
                              using T = std::remove_cvref_t<decltype(val)>;
                              if constexpr (std::integral<T>) {
                                  std::string x = std::format("{}", val);
                                  return std::to_string(val);
                              } else if constexpr (std::same_as<T, std::string> || std::same_as<T, std::string_view>) {
                                  return std::string(val);
                              }
                              return ""s;
                          }},
                        value);
            parameters.push_back(std::tuple<std::string,std::string>(key, val_s));
        }
        for (const auto& item : block_mod->inputMetaInfos()) {
            inputs.push_back(std::tuple<std::string,std::string>(item.signal_name.value, item.data_type.value));
        }
        for (const auto& item : block_mod->outputMetaInfos()) {
            outputs.push_back(std::tuple<std::string,std::string>(item.signal_name.value, item.data_type.value));
        }

        j["parameters"] = parameters;
        j["inputs"] = inputs;
        j["outputs"] = outputs;

        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
