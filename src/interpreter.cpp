#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "fingerprints/fingerprint.hpp"
#include "world/instructions.hpp"
#include "world/world.hpp"
#include "strings.hpp"

#ifdef _WIN32
#include <windows.h>
extern char** _environ;
#define environ _environ
UINT originalInCP, originalOutCP;

/// Sets the console to use UTF-8 instead of ASCII (or whatever else it was set to).
void setupConsole() {
    originalInCP = GetConsoleCP();
    originalOutCP = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
}

/// Restores the codepoint system the console was using before.
void restoreConsole() {
    SetConsoleCP(originalInCP);
    SetConsoleOutputCP(originalOutCP);
}

#elifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())

#else
extern char** environ;

#endif

void quit(const int code) {
    std::cout << "\n\n\033[0m================================================================\n"
              << " Program has finished running.\n Exit code: " << code << "\n"
              << "================================================================" << std::endl;

    #ifdef _WIN32
        restoreConsole();
    #endif

    std::exit(code);
}

int main(const int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    if(argc == 1 || std::string(argv[1]) == "--help") {
        std::cout << "\nHello! Thank you for checking out my lil' project. :)\n"
                  << "This is a Funge98 interpreter. Funge98 is an esoteric programming language. If you are not already\n"
                  << "familiar with Funge98, you can learn about it on my Github page...\n"
                  << "\033[93mhttps://github.com/ItsSkyWasTaken/Funge98-Interpreter\033[0m\n\n"
                  << "or many other sources like this wiki:\n"
                  << "\033[93mhttps://esolangs.org/wiki/Funge-98\033[0m\n\n"
                  << "To run a Funge98 file, simply pass the file as an argument when running this command:\n"
                  << "\033[96m" << argv[0] << " example.b98\033[0m\n\n"
                  << "This interpreter automatically detects if the file is a Unefunge, Befunge, or Trefunge program based\n"
                  << "on the dimensions in the file. To override this, you can specify dimensions with \033[96m--dim=VALUE\033[0m:\n"
                  << "\033[96m" << argv[0] << " example.b98 --dim=3\033[0m\n\n"
                  << "The input (i), output (o), and execute (=) commands are disabled by default. If your Funge98 file\n"
                  << "needs them, you must enable them with \033[96m-r\033[0m, \033[96m-w\033[0m, and \033[96m-e\033[0m, respectively. For example:\n"
                  << "\033[96m" << argv[0] << " example.b98 -r -w\033[0m\n\n"
                  << "\033[91m" << "These permissions allow the Funge98 file to potentially make changes to your device. Make sure you\n"
                  << "trust the file before enabling these!\033[0m\n"
                  << "Developers should use \033[96m1y\033[0m in their programs to check if these are enabled and respond with alternatives\n"
                  << "if they are not.\n\n"
                  << "To pass extra arguments into the Funge98 program, simply list them after the \033[96m--args\033[0m flag:\n"
                  << "\033[96m" << argv[0] << " example.b98 -r -w --args Example \"Example with multiple words\"\033[0m\n\n";
        return 0;
    }

    bool write = false, read = false, execute = false;
    int dim = 0;
    bool argFlag = false;
    std::vector<std::u32string> args;
    args.push_back(Strings::fromUtf8(argv[1]));

    for(int i = 2; i < argc; i++) {
        if(argFlag) {
            args.push_back(Strings::fromUtf8(argv[i]));
            continue;
        }

        if(std::string arg(argv[i]); arg == "--write" || arg == "-w" || arg == "-o") {
            write = true;
        } else if(arg == "--read" || arg == "-r" || arg == "-i") {
            read = true;
        } else if(arg == "--execute" || arg == "-e") {
            execute = true;
        } else if(arg.starts_with("--dim=")) {
            dim = arg[6] - '0';
        } else if(arg == "--args") {
            argFlag = true;
        }
    }

    std::cout << "\n\033[0m================================================================\n"
              << " Loading Funge world..." << std::endl;

    std::shared_ptr<FungeWorld> world;
    std::ifstream file(argv[1]);
    if(file.is_open()) {
        if(dim > 0 && dim < 4) {
            world = FungeWorld::fromFile(file, static_cast<int8_t>(dim));
        } else {
            world = FungeWorld::fromFile(file);
        }
    } else {
        std::cout << " [ERROR] Could not read file: " << argv[1] << "\n"
                  << "================================================================\n\n";
        return 1;
    }

    file.close();

    world->setReadEnabled(read);
    world->setWriteEnabled(write);
    world->setExecuteEnabled(execute);
    for(const std::u32string& arg : args) {
        world->passArg(arg);
    }

    for(size_t i = 0; environ[i] != nullptr; i++) {
        world->passEnvar(Strings::fromUtf8(environ[i]));
    }

    InstructionSet::load(world);
    Fingerprint::load(world);

    #ifdef _WIN32
        setupConsole();
    #endif

    std::cout << " Starting!\n"
              << "================================================================\n" << std::endl;

    world->run();
}