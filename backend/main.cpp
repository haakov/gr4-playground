
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



int main(int argc, char** argv) {
    std::println(stderr, "Setting up registry");

    // TODO: Remove when GR gets proper blocks library
    auto* registry = grGlobalBlockRegistry();
    gr::blocklib::initGrBasicBlocks(*registry);
    gr::blocklib::initGrElectricalBlocks(*registry);
    gr::blocklib::initGrFileIoBlocks(*registry);
    gr::blocklib::initGrFilterBlocks(*registry);
    gr::blocklib::initGrFourierBlocks(*registry);
    gr::blocklib::initGrHttpBlocks(*registry);
    gr::blocklib::initGrMathBlocks(*registry);
    gr::blocklib::initGrTestingBlocks(*registry);
    std::println("Blocks in registry: {}", registry->keys().size());

    std::println(stderr, "Registry done.");

    return 0;
}
