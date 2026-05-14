#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "world/world.hpp"

FungeWorld* world;

void quit(const int code) {
    delete world;

    std::cout << std::endl << std::endl << "\033[0m================================================================" << std::endl;
    std::cout << "  Program has finished running." << std::endl << "  Exit code: " << code << std::endl;
    std::cout << "================================================================" << std::endl;

    std::exit(code);
}

int main(const int argc, char** argv) {
    bool write = false, read = false, execute = false;
    int dim = 0;

    for(int i = 2; i < argc; i++) {
        if(std::string arg(argv[i]); arg == "--write" || arg == "-w") {
            write = true;
        } else if(arg == "--read" || arg == "-r") {
            read = true;
        } else if(arg == "--execute" || arg == "-e") {
            execute = true;
        } else if(arg.starts_with("--dim=")) {
            dim = arg[6] - '0';
        }
    }

    std::cout << std::endl << "\033[0m================================================================" << std::endl;
    std::cout << "  Loading Funge world..." << std::endl;

    if(std::ifstream file(argv[1]); file.is_open()) {
        if(dim > 0 && dim < 4) {
            world = FungeWorld::fromFile(file, static_cast<int8_t>(dim));
        } else {
            world = FungeWorld::fromFile(file);
        }
    } else {
        std::cerr << "Could not read file: " << argv[1] << std::endl;
        std::exit(2);
    }

    FungeWorld& w = *world;

    w.setReadEnabled(read);
    w.setWriteEnabled(write);
    w.setExecuteEnabled(execute);
    InstructionSet::load(w);

    std::cout << "  Starting!" << std::endl;
    std::cout << "================================================================" << std::endl << std::endl;

    w.start();
}