#include <iostream>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>
#include <csignal>
#include <emscripten/bind.h>

#include <gnuradio-4.0/BlockRegistry.hpp>
#include <gnuradio-4.0/Scheduler.hpp>

#include <GrBasicBlocks.hpp>
#include <GrTestingBlocks.hpp>
#include <GrFileIoBlocks.hpp>
#include <GrFilterBlocks.hpp>

#include <gnuradio-4.0/basic/ConverterBlocks.hpp>
#include <gnuradio-4.0/basic/SignalGenerator.hpp>

#include "json.hpp"

using json = nlohmann::json;

using namespace emscripten;

void signal_handler(int signum)
{
    std::cout << "\n\n SIGINT received.\n";
    exit(signum);
}

std::string blocks()
{
    auto *registry = grGlobalBlockRegistry();
    gr::blocklib::initGrBasicBlocks(*registry);
    gr::blocklib::initGrTestingBlocks(*registry);
    gr::blocklib::initGrFileIoBlocks(*registry);
    gr::blocklib::initGrFilterBlocks(*registry);
    json j;
    j["blocks"] = json::array();
    j["categories"] = json::array();

    for (auto key : registry->keys())
    {
        gr::property_map map_;
        auto block_mod = registry->create(key, map_);
        block_mod->settings().init();
        json block = {{"key", key}, {"label", key.substr(0, key.find("<"))}, {"id", key}, {"category", ""}, {"parameters", json::array()}, {"flags", json::array()}, {"inputs", json::array()}, {"outputs", json::array()}};

        for (const auto &item : block_mod->inputMetaInfos())
        {
            block["inputs"].push_back({{"key", item.signal_name.value}, {"id", item.signal_name.value}, {"optional", false}, {"type", item.data_type.value}});
        }

        for (const auto &item : block_mod->outputMetaInfos())
        {
            block["outputs"].push_back({{"key", item.signal_name.value}, {"id", item.signal_name.value}, {"optional", false}, {"type", item.data_type.value}});
        }

        for (const auto &[key_, value] : block_mod->settings().defaultParameters())
        {
            std::string val_s = value.value_or(std::string());

            auto &meta = block_mod->metaInformation();
            std::pmr::string full_key{std::format("{}::visible", key_)};

            bool visible = false;

            if (meta.contains(full_key))
            {
                auto vis = meta.at(full_key).get_if<bool>();
                visible = *vis;
            }
            block["parameters"].push_back({{"key", key_}, {"visible", visible}, {"value", val_s}, {"default", val_s}, {"hide", !visible}, {"id", key_}, {"label", key_}});

            if (key_ == "name")
            {
                if (val_s.starts_with("gr::basic"))
                {
                    block["category"] = "gr::basic";
                }
                else if (val_s.starts_with("gr::testing"))
                {
                    block["category"] = "gr::testing";
                }
                else if (val_s.starts_with("gr::blocks"))
                {
                    block["category"] = "gr::blocks";
                }
                else if (val_s.starts_with("gr::filter"))
                {
                    block["category"] = "gr::filter";
                }
                else
                {
                    block["category"] = "other";
                }
            }
        }

        auto category = block["category"];
        j["blocks"].push_back(block);
        if (!std::ranges::contains(j["categories"], category))
        {
            j["blocksByCategory"][category] = json::array();
            j["categories"].push_back(category);
        }
        j["blocksByCategory"][category].push_back(block);
    }
    j["total_blocks"] = j["blocks"].size();
    j["generated_at"] = std::string("10:00 01.01.2026");
    std::println("Blocks");
    return j.dump();
}


int main()
{

    signal(SIGINT, signal_handler); // Catch SIGINT
    return 0;
}

EMSCRIPTEN_BINDINGS(main)
{
    function("blocks", &blocks);
}
