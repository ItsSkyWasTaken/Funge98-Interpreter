#include <fstream>
#include <iostream>
#include <regex>

#include "world/world.hpp"

FungeWorld* world;

int main(const int argc, char** argv) {
    const std::string filename = argv[1];
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
            std::smatch match;
            std::regex_search(arg, match, std::regex("--dim=(\\d)"));
            dim = stoi(match[1].str());
        }
    }

    std::ifstream file(filename);

    if(dim > 0 && dim < 4) {
        world = FungeWorld::fromFile(file, static_cast<int8_t>(dim));
    } else {
        world = FungeWorld::fromFile(file);
    }

    FungeWorld& w = *world;

    w.setReadEnabled(read);
    w.setWriteEnabled(write);
    w.setExecuteEnabled(execute);
    InstructionSet::load(w);

    w.start();

    delete world;
}