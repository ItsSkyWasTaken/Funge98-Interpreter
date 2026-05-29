#include <fstream>
#include <iostream>
#include <string>

#include "world/world.hpp"

#ifdef _WIN32
    #include <windows.h>
    extern char** _environ;
    #define environ _environ
    UINT originalInCP, originalOutCP;
#elifdef __APPLE__
    #include <crt_externs.h>
    #define environ (*_NSGetEnviron())
#else
    extern char** environ;
#endif

void setupConsole() {
    #ifdef _WIN32
        originalInCP = GetConsoleCP();
        originalOutCP = GetConsoleOutputCP();
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
}

void restoreConsole() {
    #ifdef _WIN32
        SetConsoleCP(originalInCP);
        SetConsoleOutputCP(originalOutCP);
    #endif
}

FungeWorld* world;

void quit(const int code) {
    delete world;

    std::cout << std::endl << std::endl << "\033[0m================================================================" << std::endl
              << " Program has finished running." << std::endl << " Exit code: " << code << std::endl
              << "================================================================" << std::endl;

    restoreConsole();
    std::exit(code);
}

std::string toUtf8(const std::u32string& s) {
    std::string result;
    for(const char32_t c : s) {
        if(c <= 0x7F) {
            result += static_cast<char>(c);
        } else if(c <= 0x7FF) {
            result += static_cast<char>(0xC0 | c >> 6 & 0x1F);
            result += static_cast<char>(0x80 | c & 0x3F);
        } else if(c <= 0xFFFF) {
            result += static_cast<char>(0xE0 | c >> 12 & 0x0F);
            result += static_cast<char>(0x80 | c >> 6 & 0x3F);
            result += static_cast<char>(0x80 | c & 0x3F);
        } else if(c <= 0x10FFFF) {
            result += static_cast<char>(0xF0 | c >> 18 & 0x07);
            result += static_cast<char>(0x80 | c >> 12 & 0x3F);
            result += static_cast<char>(0x80 | c >> 6 & 0x3F);
            result += static_cast<char>(0x80 | c & 0x3F);
        }
    }
    return result;
}

std::u32string fromUtf8(const std::string& s) {
    std::u32string result;
    size_t i = 0;

    while(i < s.length()) {
        const auto b = static_cast<uint8_t>(s[i]);
        char32_t c = 0;
        size_t bytesToRead = 0;

        if(b <= 0x7F) {
            c = b;
            bytesToRead = 0;
        } else if((b & 0xE0) == 0xC0) {
            c = b & 0x1F;
            bytesToRead = 1;
        } else if((b & 0xF0) == 0xE0) {
            c = b & 0x0F;
            bytesToRead = 2;
        } else if((b & 0xF8) == 0xF0) {
            c = b & 0x07;
            bytesToRead = 3;
        } else {
            c = 0xFFFD;
            bytesToRead = 0;
        }

        if(i + bytesToRead >= s.length()) {
            result += 0xFFFD;
            break;
        }

        for(size_t j = 0; j < bytesToRead; j++) {
            i++;
            if (const auto next = static_cast<uint8_t>(s[i]); (next & 0xC0) == 0x80) {
                c = c << 6 | next & 0x3F;
            } else {
                c = 0xFFFD;
                break;
            }
        }

        result += c;
        i++;
    }
    return result;
}

int main(const int argc, char** argv) {
    if(argc == 1 || std::string(argv[1]) == "--help") {
        std::cout << std::endl << "Hello! Thank you for checking out my lil' project. :)" << std::endl
                  << "This is a Funge98 interpreter. Funge98 is an esoteric programming language. If you are not already" << std::endl
                  << "familiar with Funge98, you can learn about it on my Github page..." << std::endl
                  << "\033[93mhttps://github.com/ItsSkyWasTaken/Funge98-Interpreter\033[0m" << std::endl << std::endl
                  << "or many other sources like this wiki:" << std::endl
                  << "\033[93mhttps://esolangs.org/wiki/Funge-98\033[0m" << std::endl << std::endl
                  << "To run a Funge98 file, simply pass the file as an argument when running this command:" << std::endl
                  << "\033[96m" << argv[0] << " example.b98" << "\033[0m" << std::endl << std::endl
                  << "This interpreter automatically detects if the file is a Unefunge, Befunge, or Trefunge program based" << std::endl
                  << "on the dimensions in the file. To override this, you can specify dimensions with \033[96m--dim=VALUE\033[0m:" << std::endl
                  << "\033[96m" << argv[0] << " example.b98 --dim=3" << "\033[0m" << std::endl << std::endl
                  << "The input (i), output (o), and execute (=) commands are disabled by default. If your Funge98 file" << std::endl
                  << "needs them, you must enable them with \033[96m-r\033[0m, \033[96m-w\033[0m, and \033[96m-e\033[0m, respectively. For example:" << std::endl
                  << "\033[96m" << argv[0] << " example.b98 -r -w" << "\033[0m" << std::endl << std::endl
                  << "\033[91m" << "These permissions allow the Funge98 file to potentially make changes to your device. Make sure you " << std::endl
                  << "trust the file before enabling these!" << "\033[0m" << std::endl
                  << "Developers should use \033[96m1y\033[0m in their programs to check if these are enabled and respond with alternatives" << std::endl
                  << "if they are not." << std::endl << std::endl
                  << "To pass extra arguments into the Funge98 program, simply list them after the \033[96m--args\033[0m flag:" << std::endl
                  << "\033[96m" << argv[0] << " example.b98 -r -w --args Example \"Example with multiple words\"" << "\033[0m" << std::endl << std::endl;
        return 0;
    }

    bool write = false, read = false, execute = false;
    int dim = 0;
    bool argFlag = false;
    std::vector<std::u32string> args;
    std::vector<std::u32string> envars;
    args.push_back(fromUtf8(argv[1]));

    for(int i = 2; i < argc; i++) {
        if(argFlag) {
            args.push_back(fromUtf8(argv[i]));
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

    std::cout << std::endl << "\033[0m================================================================" << std::endl
              << " Loading Funge world..." << std::endl;

    if(std::ifstream file(argv[1]); file.is_open()) {
        if(dim > 0 && dim < 4) {
            world = FungeWorld::fromFile(file, static_cast<int8_t>(dim));
        } else {
            world = FungeWorld::fromFile(file);
        }
    } else {
        std::cout << " [ERROR] Could not read file: " << argv[1] << std::endl
                  << "================================================================" << std::endl << std::endl;
        return 2;
    }

    FungeWorld& w = *world;

    w.setReadEnabled(read);
    w.setWriteEnabled(write);
    w.setExecuteEnabled(execute);
    for(const std::u32string& arg : args) {
        w.passArg(arg);
    }

    for(size_t i = 0; environ[i] != nullptr; i++) {
        envars.push_back(fromUtf8(environ[i]));
    }

    InstructionSet::load(w);
    setupConsole();

    std::cout << " Starting!" << std::endl
              << "================================================================" << std::endl << std::endl;

    w.run();
}