#include <algorithm>
#include <array>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>

#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/Scheduler.hpp>

#include <GrBasicBlocks.hpp>
#include <GrTestingBlocks.hpp>

#include <gnuradio-4.0/basic/ConverterBlocks.hpp>
#include <gnuradio-4.0/basic/SignalGenerator.hpp>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std::chrono_literals;

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

    svr.set_exception_handler([](const auto& req, auto& res, std::exception_ptr ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception& e) {
            std::cerr << "Server Exception: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("Internal Server Error", "text/plain");
        } catch (...) {
            std::cerr << "Unknown Server Exception" << std::endl;
            res.status = 500;
            res.set_content("Internal Server Error", "text/plain");
        }
    });

    signal(SIGINT, signal_handler); // Catch SIGINT
    auto* registry = grGlobalBlockRegistry();
    gr::blocklib::initGrBasicBlocks(*registry);
    gr::blocklib::initGrTestingBlocks(*registry);

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
        json j;
        j["blocks"]     = json::array();
        j["categories"] = json::array();

        for (auto key : registry->keys()) {
            gr::property_map map_;
            auto             block_mod = registry->create(key, map_);
            block_mod->settings().init();
            json block = {{"key", key}, {"label", key.substr(0, key.find("<"))}, {"id", key}, {"category", ""}, {"parameters", json::array()}, {"flags", json::array()}, {"inputs", json::array()}, {"outputs", json::array()}};

            for (const auto& item : block_mod->inputMetaInfos()) {
                block["inputs"].push_back({{"key", item.signal_name.value}, {"id", item.signal_name.value}, {"optional", false}, {"type", item.data_type.value}});
            }

            for (const auto& item : block_mod->outputMetaInfos()) {
                block["outputs"].push_back({{"key", item.signal_name.value}, {"id", item.signal_name.value}, {"optional", false}, {"type", item.data_type.value}});
            }

            for (const auto& [key_, value] : block_mod->settings().defaultParameters()) {
                std::string val_s = value.value_or(std::string());

                auto&            meta = block_mod->metaInformation();
                std::pmr::string full_key{std::format("{}::visible", key_)};

                bool visible = false;

                if (meta.contains(full_key)) {
                    auto vis = meta.at(full_key).get_if<bool>();
                    visible  = *vis;
                }
                block["parameters"].push_back({{"key", key_}, {"visible", visible}, {"value", val_s}, {"default", val_s}, {"hide", !visible}, {"id", key_}, {"label", key_}});

                if (key_ == "name") {
                    if (val_s.starts_with("gr::basic")) {
                        block["category"] = "gr::basic";
                    } else if (val_s.starts_with("gr::testing")) {
                        block["category"] = "gr::testing";
                    } else if (val_s.starts_with("gr::blocks")) {
                        block["category"] = "gr::blocks";
                    } else {
                        block["category"] = "other";
                    }
                }
            }

            auto category = block["category"];
            j["blocks"].push_back(block);
            if (!std::ranges::contains(j["categories"], category)) {
                j["blocksByCategory"][category] = json::array();
                j["categories"].push_back(category);
            }
            j["blocksByCategory"][category].push_back(block);
        }
        j["total_blocks"] = j["blocks"].size();
        j["generated_at"] = std::string("10:00 01.01.2026");
        std::println("Blocks");

        res.set_content(j.dump(), "application/json");
    });

    svr.Options("/run", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        res.status = 204;
    });
    svr.Post("/run", [](const httplib::Request& req, httplib::Response& res) {
        enableCORS(res);
        // json                       data    = json::parse(req.body);
        // std::string                block   = data["flowgraph"];
        constexpr std::string_view testGrc = R"(
blocks:
  - id: gr::testing::NullSink<float32>
    parameters:
      name: NullSink<float32>
  - id: gr::testing::NullSource<float32>
    parameters:
      name: NullSource<float32>

connections:
  - [NullSource<float32>, 0, NullSink<float32>, 0]
)";
        auto                       graph   = gr::loadGrc(gr::globalPluginLoader(), testGrc);
        gr::scheduler::Simple      sched;
        if (auto ret = sched.exchange(std::move(graph)); !ret) {
            throw std::runtime_error(std::format("failed to initialize scheduler: {}", ret.error()));
        }
        std::chrono::milliseconds timeout = 3s;
        std::atomic<bool>         schedulerDone{false};
        std::thread               timeoutThread([&sched, &schedulerDone, timeout] {
            const auto deadline = std::chrono::steady_clock::now() + timeout;
            while (!schedulerDone.load() && std::chrono::steady_clock::now() < deadline) {
                std::this_thread::sleep_for(10ms);
            }
            if (!schedulerDone.load()) {
                sched.requestStop();
                std::println("requesting stop");
            }
        });
        auto                      result = sched.runAndWait();
        schedulerDone                    = true;
        timeoutThread.join();
        res.set_content("\"result\": true", "application/json");
    });

    std::cout << "Server started at localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
