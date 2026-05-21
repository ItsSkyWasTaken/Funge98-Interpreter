// ReSharper disable CppDFANullDereference
#include "world.hpp"

#include <chrono>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

std::unordered_map<char32_t, std::function<bool(InstructionPointer&)>> InstructionSet::commands;
FungeWorld* InstructionSet::world = nullptr;
int InstructionSet::pointerPosition = 0;
int InstructionSet::ansiFlag = 0;
int InstructionSet::ansiParameter = 0;

void InstructionSet::load(FungeWorld& w) {
    world = &w;

    commands[U'!'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        stack.push(!stack.pop());
        return true;
    };

    commands[U'"'] = [](InstructionPointer& ip) {
        ip.setPointerState(PointerState::STRING);
        return true;
    };

    commands[U'#'] = [](InstructionPointer& ip) {
        ip.advance(1);
        return true;
    };

    commands[U'$'] = [](const InstructionPointer& ip) {
        ip.getStack().pop();
        return true;
    };

    commands[U'%'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(a % b);
        return true;
    };

    commands[U'&'] = [](const InstructionPointer& ip) {
        std::string input;
        int32_t n;

        getline(std::cin, input);
        std::istringstream iss(input);

        while(!(iss >> n)) {
            std::cin.clear();
            std::cout << "\033[1F\033[" << pointerPosition << "C\033[0J";

            getline(std::cin, input);
            iss = std::istringstream(input);
        }

        ip.getStack().push(n);
        return true;
    };

    commands[U'\''] = [](InstructionPointer& ip) {
        ip.advance(1);
        ip.getStack().push(world->get(ip.getLocation()));
        return true;
    };

    commands[U'*'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when multiplying.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast product back to a signed integer.
        stack.push(static_cast<int32_t>(a * b));
        return true;
    };

    commands[U'+'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when adding.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast sum back to a signed integer.
        stack.push(static_cast<int32_t>(a + b));
        return true;
    };

    commands[U','] = [](const InstructionPointer& ip) {
        const char32_t c = ip.getStack().popChar();
        const std::u32string s(1, c);
        std::cout << toUtf8(s);

        switch(ansiFlag) {
            case 2:
                if(c >= 64 && c < 127) {
                    ansiFlag = 0;

                    switch(c) {
                        case U'C':
                            pointerPosition += ansiParameter;
                            break;
                        case U'D':
                            pointerPosition -= std::min(ansiParameter, pointerPosition);
                            break;
                        case U'E':
                        case U'F':
                            pointerPosition = 0;
                            break;
                        case U'G':
                            pointerPosition = ansiParameter - 1;
                            break;
                        default:
                            // ignore
                            break;
                    }

                    ansiParameter = 0;
                } else if(c >= 48 && c < 58) {
                    ansiParameter *= 10;
                    ansiParameter += static_cast<int>(c - '0');
                }
                break;
            case 1:
                if(c == U'[') {
                    ansiFlag = 2;
                    return true;
                }

                ansiFlag = 0;
            default:
                if(c > 9 && c < 13) {
                    pointerPosition = 0;
                } else if(c == 8) {
                    if(pointerPosition > 0) {
                        pointerPosition -= 1;
                    }
                } else if(c == 9) {
                    const int space = 8 - pointerPosition % 8;
                    pointerPosition += space == 0 ? 8 : space;
                } else if(c == 27) {
                    ansiFlag = 1;
                } else if(c >= 32 && c != 127) {
                    pointerPosition += 1;
                }
        }

        return true;
    };

    commands[U'-'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when subtracting.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast difference back to a signed integer.
        stack.push(static_cast<int32_t>(a - b));
        return true;
    };

    commands[U'.'] = [](const InstructionPointer& ip) {
        const int32_t n = ip.getStack().pop();
        std::cout << n << " ";
        pointerPosition += n == 0 ? 2 : static_cast<int>(log10(abs(n))) + 2 + (n < 0 ? 1 : 0);
        return true;
    };

    commands[U'/'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(a / b);
        return true;
    };

    commands[U'0'] = [](const InstructionPointer& ip) {
        ip.getStack().push(0);
        return true;
    };

    commands[U'1'] = [](const InstructionPointer& ip) {
        ip.getStack().push(1);
        return true;
    };

    commands[U'2'] = [](const InstructionPointer& ip) {
        ip.getStack().push(2);
        return true;
    };

    commands[U'3'] = [](const InstructionPointer& ip) {
        ip.getStack().push(3);
        return true;
    };

    commands[U'4'] = [](const InstructionPointer& ip) {
        ip.getStack().push(4);
        return true;
    };

    commands[U'5'] = [](const InstructionPointer& ip) {
        ip.getStack().push(5);
        return true;
    };

    commands[U'6'] = [](const InstructionPointer& ip) {
        ip.getStack().push(6);
        return true;
    };

    commands[U'7'] = [](const InstructionPointer& ip) {
        ip.getStack().push(7);
        return true;
    };

    commands[U'8'] = [](const InstructionPointer& ip) {
        ip.getStack().push(8);
        return true;
    };

    commands[U'9'] = [](const InstructionPointer& ip) {
        ip.getStack().push(9);
        return true;
    };

    commands[U':'] = [](const InstructionPointer& ip) {
        ip.getStack().duplicate();
        return true;
    };

    commands[U'<'] = [](InstructionPointer& ip) {
        ip.setDelta(-1, 0, 0);
        return true;
    };

    commands[U'='] = [](const InstructionPointer& ip) {
        if(!world->canExecute()) {
            return false;
        }

        const std::u32string command = ip.getStack().popString();
        const char* converted = toUtf8(command).c_str();
        const int32_t status = std::system(converted);
        ip.getStack().push(status);
        return true;
    };

    commands[U'>'] = [](InstructionPointer& ip) {
        ip.setDelta(1, 0, 0);
        return true;
    };

    commands[U'?'] = [](InstructionPointer& ip) {
        ip.setDelta(Vector::random(world->dimensions));
        return true;
    };

    commands[U'@'] = [](InstructionPointer& ip) {
        ip.setPointerState(PointerState::EXITING);
        return true;
    };

    commands[U'['] = [](InstructionPointer& ip) {
        const Vector& delta = ip.getDelta();
        if(delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(delta.getY(), -delta.getX(), delta.getZ());
        return true;
    };

    commands[U'\\'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(b);
        stack.push(a);
        return true;
    };

    commands[U']'] = [](InstructionPointer& ip) {
        const Vector& delta = ip.getDelta();
        if(delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(-delta.getY(), delta.getX(), delta.getZ());
        return true;
    };

    commands[U'^'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, -1, 0);
        return true;
    };

    commands[U'_'] = [](InstructionPointer& ip) {
        ip.setDelta(ip.getStack().pop() ? -1 : 1, 0, 0);
        return true;
    };

    commands[U'`'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(a > b);
        return true;
    };

    commands[U'a'] = [](const InstructionPointer& ip) {
        ip.getStack().push(10);
        return true;
    };

    commands[U'b'] = [](const InstructionPointer& ip) {
        ip.getStack().push(11);
        return true;
    };

    commands[U'c'] = [](const InstructionPointer& ip) {
        ip.getStack().push(12);
        return true;
    };

    commands[U'd'] = [](const InstructionPointer& ip) {
        ip.getStack().push(13);
        return true;
    };

    commands[U'e'] = [](const InstructionPointer& ip) {
        ip.getStack().push(14);
        return true;
    };

    commands[U'f'] = [](const InstructionPointer& ip) {
        ip.getStack().push(15);
        return true;
    };

    commands[U'g'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        stack.push(world->get(ip.getOffset() + stack.popVector(world->dimensions)));
        return true;
    };

    commands[U'h'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, -1);
        return true;
    };

    // TODO: implement input command
    commands[U'i'] = [](InstructionPointer& _) {
        return false;
    };

    commands[U'j'] = [](InstructionPointer& ip) {
        ip.advance(ip.getStack().pop());
        return true;
    };

    commands[U'k'] = [](InstructionPointer& ip) {
        const Vector old = ip.getLocation();
        const int32_t n = ip.getStack().pop();

        if(n < 0) {
            return false;
        }

        ip.advance(1);
        auto c = static_cast<uint32_t>(world->get(ip.getLocation()));

        Retry:
        while(c > 126 || c <= 32) {
            ip.advance(1);
            c = static_cast<uint32_t>(world->get(ip.getLocation()));

            if(!world->boundsCheck(ip)) {
                ip.setLocation(old);
                return false;
            }
        }

        if(c == U';') {
            ip.advance(1);
            c = static_cast<uint32_t>(world->get(ip.getLocation()));
            while(c != U';') {
                ip.advance(1);
                c = static_cast<uint32_t>(world->get(ip.getLocation()));

                if(!world->boundsCheck(ip)) {
                    ip.setLocation(old);
                    return false;
                }
            }

            ip.advance(1);
            c = static_cast<uint32_t>(world->get(ip.getLocation()));
            goto Retry;
        }

        if(supports(c)) {
            if(n == 0) {
                return true;
            }

            for(int i = 0; i < n; i++) {
                if(!execute(ip)) {
                    return false;
                }
            }

            return true;
        }

        ip.setLocation(old);
        return false;
    };

    commands[U'l'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, 1);
        return true;
    };

    commands[U'm'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, ip.getStack().pop() ? -1 : 1);
        return true;
    };

    commands[U'n'] = [](const InstructionPointer& ip) {
        ip.getStack().clear();
        return true;
    };

    // TODO: implement output command
    commands[U'o'] = [](InstructionPointer& _) {
        return false;
    };

    commands[U'p'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const Vector& v = stack.popVector(world->dimensions);
        const int32_t n = stack.pop();
        world->put(ip.getOffset() + v, n);
        return true;
    };

    commands[U'q'] = [](const InstructionPointer& ip) {
        quit(ip.getStack().pop());
        return true;
    };

    commands[U'r'] = [](InstructionPointer& ip) {
        ip.setDelta(ip.getDelta() * -1);
        return true;
    };

    commands[U's'] = [](const InstructionPointer& ip) {
        world->put(ip.getLocation() + ip.getDelta(), ip.getStack().popChar());
        return true;
    };

    commands[U't'] = [](const InstructionPointer& ip) {
        InstructionPointer* ip2 = ip.split();
        ip2->advance(1);
        world->pointers.push(ip2);
        return true;
    };

    commands[U'u'] = [](const InstructionPointer& ip) {
        if(ip.getStack().size() < 2) {
            return false;
        }

        ip.getStack().transfer(ip.getStack().pop());
        return true;
    };

    commands[U'v'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, 1, 0);
        return true;
    };

    commands[U'w'] = [](InstructionPointer& ip) {
        const Vector& delta = ip.getDelta();
        if(delta.dimensions == 1) {
            return false;
        }

        Stack& stack = ip.getStack();
        if(const int32_t b = stack.pop(), a = stack.pop(); a > b) {
            ip.setDelta(-delta.getY(), delta.getX(), delta.getZ());
        } else if(a < b) {
            ip.setDelta(delta.getY(), -delta.getX(), delta.getZ());
        }

        return true;
    };

    commands[U'x'] = [](InstructionPointer& ip) {
        ip.setDelta(ip.getStack().popVector(ip.getDelta().dimensions));
        return true;
    };

    commands[U'y'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t c = stack.pop();
        const std::vector<int32_t> sizes = ip.stackSizes();

        switch(c) {
            // If out of range, push everything
            default:

            // Case 20: push environment variables onto the stack as null-terminated strings (unordered); the end is
            // terminated by another null character.
            case 20: {
                stack.push(0);
                for(const std::u32string& envar : world->envars) {
                    stack.push(envar);
                }

                if(c == 20) break;
            }

            // Case 19: push arguments onto the stack as null-terminated strings; the end is terminated by another null.
            case 19: {
                stack.push(0);
                for(const std::u32string& arg : world->args | std::views::reverse) {
                    stack.push(arg);
                }

                if(c == 19) break;
            }

            // Case 18: push the sizes of each stack in the SS; TOSS is on top.
            case 18: {
                for(const int32_t size : sizes | std::views::reverse) {
                    stack.push(size);
                }
                if(c == 18) break;
            }

            // Case 17: push the number of stacks in the SS onto the stack.
            case 17: {
                stack.push(static_cast<int32_t>(sizes.size()));
                if(c == 17) break;
            }

            // Case 16: push current time as an int in the form of (hours * 65536) + (minutes * 256) + (seconds).
            case 16: {
                const std::time_t t = std::time(nullptr);
                const std::tm* localTime = std::localtime(&t);
                stack.push(localTime->tm_hour * 65536 + localTime->tm_min * 256 + localTime->tm_sec);

                if(c == 16) break;
            }

            // Case 15: push current date as an int in the form (yearsSince1900 * 65536) + (month * 256) + (dayOfMonth).
            case 15: {
                const std::time_t t = std::time(nullptr);
                const std::tm* localTime = std::localtime(&t);
                stack.push(localTime->tm_year * 65536 + (localTime->tm_mon + 1) * 256 + localTime->tm_mday);

                if(c == 15) break;
            }

            // Case 14: push high corner of region with non-space cells, relative to low the corner.
            case 14: {
                stack.push(world->high - world->low);
                if(c == 14) break;
            }

            // Case 13: push low corner of region with non-space cells, relative to the origin.
            case 13: {
                stack.push(world->low);
                if(c == 13) break;
            }

            // Case 12: push pointer's storage offset.
            case 12: {
                stack.push(ip.getOffset());
                if(c == 12) break;
            }

            // Case 11: push pointer's delta.
            case 11: {
                stack.push(ip.getDelta());
                if(c == 11) break;
            }

            // Case 10: push pointer's location.
            case 10: {
                stack.push(ip.getLocation());
                if(c == 10) break;
            }

            // Case 9: push pointer's team number (not relevant in this interpreter; no networking capabilities yet).
            case 9: {
                stack.push(0);
                if(c == 9) break;
            }

            // Case 8: push pointer's unique ID.
            case 8: {
                stack.push(ip.id);
                if(c == 8) break;
            }

            // Case 7: push number of world dimensions.
            case 7: {
                stack.push(world->dimensions);
                if(c == 7) break;
            }

            // Case 6: push file separator character.
            case 6: {
                stack.push(static_cast<char32_t>(std::filesystem::path::preferred_separator));
                if(c == 6) break;
            }

            // Case 5: push Operating Paradigm - equivalent to system() if enabled.
            case 5: {
                stack.push(world->canExecute() ? 1 : 0);
                if(c == 5) break;
            }

            // Case 4: push version number - v0.1.1 = 101.
            case 4: {
                stack.push(101);
                if(c == 4) break;
            }

            // Case 3: push handprint - ISWT (0x49535754).
            case 3: {
                stack.push(0x49535754);
                if(c == 3) break;
            }

            // Case 2: push number of bytes per cell - 32-bits = 4 bytes.
            case 2: {
                stack.push(4);
                if(c == 2) break;
            }

            // Case 1: push flags concerning availability of (=, o, i, t).
            case 1: {
                int32_t bits = 1;
                bits |= world->canExecute() ? 8 : 0;

                stack.push(bits);
            }
        }

        return true;
    };

    commands[U'z'] = [](InstructionPointer& _) {
        return true;
    };

    commands[U'{'] = [](InstructionPointer& ip) {
        ip.startBlock();
        return true;
    };

    commands[U'|'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, ip.getStack().pop() ? -1 : 1, 0);
        return true;
    };

    commands[U'}'] = [](InstructionPointer& ip) {
        if(ip.getStack().size() < 2) {
            return false;
        }

        ip.endBlock();
        return true;
    };

    commands[U'~'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        // char c;
        // std::cin.get(c);
        // ip.getStack().push(static_cast<char32_t>(c));

        const int first = std::cin.get();

        if(first == EOF) {
            std::cin.clear();
            return false;
        }

        const auto a = static_cast<uint8_t>(first);

        if((a & 0x80) == 0) {
            stack.push(a);

            if(a > 9 && a < 13) {
                pointerPosition = 0;
            }

            return true;
        }

        if((a & 0xE0) == 0xC0) {
            const int b = std::cin.get();
            if(b == EOF) return false;
            stack.push((a & 0x1F) << 6 | static_cast<uint8_t>(b) & 0x3F);
            return true;
        }

        if((a & 0xF0) == 0xE0) {
            const int b = std::cin.get();
            const int c = std::cin.get();
            if(b == EOF || c == EOF) return false;
            stack.push( (a & 0x0F) << 12 | (static_cast<uint8_t>(b) & 0x3F) << 6 | static_cast<uint8_t>(c) & 0x3F);
            return true;
        }

        if((a & 0xF8) == 0xF0) {
            const int b = std::cin.get();
            const int c = std::cin.get();
            const int d = std::cin.get();
            if(b == EOF || c == EOF || d == EOF) return false;
            stack.push( (a & 0x07) << 18 | (static_cast<uint8_t>(b) & 0x3F) << 12 | (static_cast<uint8_t>(c) & 0x3F) << 6 | static_cast<uint8_t>(d) & 0x3F);
            return true;
        }

        stack.push(a);
        return true;
    };
}

bool InstructionSet::supports(const char32_t command) {
    return commands.contains(command);
}

bool InstructionSet::execute(InstructionPointer& ip) {
    if(!commands[world -> get(ip.getLocation())](ip)) {
        ip.setDelta(ip.getDelta() * -1);
        return false;
    }

    return true;
}

FungeWorld* FungeWorld::fromFile(std::ifstream& file) {
    std::vector<std::vector<std::u32string>> data;
    data.emplace_back();

    size_t maxPlane = 0, maxRow = 0;
    std::string line;
    while(std::getline(file, line)) {
        if(line == "\\f") {
            if(data.back().size() > maxPlane) {
                maxPlane = data.back().size();
            }

            data.emplace_back();
        } else {
            data.back().push_back(fromUtf8(line));

            if(line.size() > maxRow) {
                maxRow = line.size();
            }
        }
    }

    if(data.back().size() > maxPlane) {
        maxPlane = data.back().size();
    }

    file.close();

    if(data.size() == 1) {
        if(data.back().size() == 1) {
            return new FungeWorld(data, Vector::origin(1), Vector(static_cast<int32_t>(maxRow - 1)));
        }

        return new FungeWorld(data, Vector::origin(1), Vector(static_cast<int32_t>(maxRow - 1), static_cast<int32_t>(maxPlane - 1)));
    }

    return new FungeWorld(data, Vector::origin(3), Vector(static_cast<int32_t>(maxRow - 1), static_cast<int32_t>(maxPlane - 1), static_cast<int32_t>(data.size() - 1)));
}

FungeWorld* FungeWorld::fromFile(std::ifstream& file, const int8_t dim) {
    std::vector<std::vector<std::u32string>> data;

    if(dim == 1) {
        std::string line;
        std::string result;
        while(std::getline(file, line)) {
            result += line;
        }

        data.emplace_back();
        data.back().push_back(fromUtf8(result));
        return new FungeWorld(data, Vector::origin(1), Vector(static_cast<int32_t>(result.size() - 1)));
    }

    if(dim == 2) {
        size_t maxRow = 0;
        data.emplace_back();
        std::string line;
        while(std::getline(file, line)) {
            data.back().push_back(fromUtf8(line));

            if(line.size() > maxRow) {
                maxRow = line.size();
            }
        }

        return new FungeWorld(data, Vector::origin(2), Vector(static_cast<int32_t>(maxRow - 1), static_cast<int32_t>(data.back().size() - 1)));
    }

    data.emplace_back();
    std::string line;
    size_t maxPlane = 0, maxRow = 0;
    while(std::getline(file, line)) {
        if(line == "\\f") {
            if(data.back().size() > maxPlane) {
                maxPlane = data.back().size();
            }

            data.emplace_back();
        } else {
            data.back().push_back(fromUtf8(line));

            if(line.size() > maxRow) {
                maxRow = line.size();
            }
        }
    }

    if(data.back().size() > maxPlane) {
        maxPlane = data.back().size();
    }

    file.close();

    return new FungeWorld(data, Vector::origin(3), Vector(static_cast<int32_t>(maxRow - 1), static_cast<int32_t>(maxPlane - 1), static_cast<int32_t>(data.size() - 1)));
}

char32_t FungeWorld::get(const Vector& location) const {
    if(location.getZ() < zStart || location.getZ() >= static_cast<int32_t>(data.size()) + zStart) {
        return U' ';
    }

    const int32_t z = location.getZ() - zStart;
    if(location.getY() < yStarts[z] || location.getY() >= static_cast<int32_t>(data[z].size()) + yStarts[z]) {
        return U' ';
    }

    const int32_t y = location.getY() - yStarts[z];
    if(location.getX() < xStarts[z][y] || location.getX() >= static_cast<int32_t>(data[z][y].size()) + xStarts[z][y]) {
        return U' ';
    }

    return data[z][y][location.getX() - xStarts[z][y]];
}

void FungeWorld::put(const Vector& location, const char32_t value) {
    if(location.dimensions > dimensions) {
        return;
    }

    if(location.getZ() < zStart) {
        data.insert(data.begin(), zStart - location.getZ(), std::vector<std::u32string>());
        yStarts.insert(yStarts.begin(), zStart - location.getZ(), 0);
        xStarts.insert(xStarts.begin(), zStart - location.getZ(), std::vector(1, 0));
        zStart = location.getZ();
        low -= Vector(0, 0, low.getZ() - location.getZ());
    } else if(location.getZ() >= static_cast<int32_t>(data.size()) + zStart) {
        data.insert(data.end(), location.getZ() - (static_cast<int32_t>(data.size()) + zStart - 1), std::vector<std::u32string>());
        yStarts.insert(yStarts.end(), zStart - location.getZ(), 0);
        xStarts.insert(xStarts.end(), zStart - location.getZ(), std::vector(1, 0));
        high += Vector(0, 0, location.getZ() - high.getZ());
    }

    const int32_t z = location.getZ() - zStart;
    if(location.getY() < yStarts[z]) {
        data[z].insert(data[z].begin(), yStarts[z] - location.getY(), std::u32string());
        xStarts[z].insert(xStarts[z].begin(), yStarts[z] - location.getY(), 0);
        yStarts[z] = location.getY();

        if(location.getY() < low.getY()) {
            low -= Vector(0, low.getY() - location.getY(), 0);
        }
    } else if(location.getY() >= static_cast<int32_t>(data[z].size()) + yStarts[z]) {
        data[z].insert(data[z].end(), location.getY() - (static_cast<int32_t>(data[z].size()) + yStarts[z] - 1), std::u32string());
        xStarts[z].insert(xStarts[z].end(), yStarts[z] - location.getY(), 0);

        if(location.getY() > high.getY()) {
            high += Vector(0, location.getY() - high.getY(), 0);
        }
    }

    const int32_t y = location.getY() - yStarts[z];
    if(location.getX() < xStarts[z][y]) {
        data[z][y].insert(0, xStarts[z][y] - location.getX(), U' ');
        xStarts[z][y] = location.getX();
    } else if(location.getX() >= static_cast<int32_t>(data[z][y].size()) + xStarts[z][y]) {
        data[z][y].append(location.getX() - (static_cast<int32_t>(data[z][y].size()) + xStarts[z][y] - 1), U' ');
    }

    data[z][y][location.getX() - xStarts[z][y]] = value;
}

void FungeWorld::setReadEnabled(const bool enabled) {
    read = enabled;
}

void FungeWorld::setWriteEnabled(const bool enabled) {
    write = enabled;
}

void FungeWorld::setExecuteEnabled(const bool enabled) {
    execute = enabled;
}

bool FungeWorld::canRead() const {
    return read;
}

bool FungeWorld::canWrite() const {
    return write;
}

bool FungeWorld::canExecute() const {
    return execute;
}

void FungeWorld::passArg(const std::u32string& arg) {
    args.push_back(arg);
}

void FungeWorld::passEnvar(const std::u32string& envar) {
    envars.push_back(envar);
}

FungeWorld::FungeWorld(const std::vector<std::vector<std::u32string>>& data, const Vector& low, const Vector& high):
        data(data), dimensions(high.dimensions), high(high), low(low), zStart(0), write(false), read(false), execute(false) {
    for(const auto& datum : data) {
        yStarts.emplace_back(0);
        xStarts.emplace_back();
        xStarts.back().assign(datum.size(), 0);
    }
}

bool FungeWorld::boundsCheck(const InstructionPointer& ip) const {
    if(ip.getLocation().getX() < low.getX() && ip.getDelta().getX() <= 0) {
        return false;
    }

    if(ip.getLocation().getY() < low.getY() && ip.getDelta().getY() <= 0) {
        return false;
    }

    if(ip.getLocation().getZ() < low.getZ() && ip.getDelta().getZ() <= 0) {
        return false;
    }

    if(ip.getLocation().getX() > high.getX() && ip.getDelta().getX() >= 0) {
        return false;
    }

    if(ip.getLocation().getY() > high.getY() && ip.getDelta().getY() >= 0) {
        return false;
    }

    if(ip.getLocation().getZ() > high.getZ() && ip.getDelta().getZ() >= 0) {
        return false;
    }

    return true;
}

void FungeWorld::start() {
    pointers.push(new InstructionPointer(dimensions));
    InstructionPointer& ip = *pointers.front();

    auto c = static_cast<uint32_t>(get(ip.getLocation()));
    while(c > 126 || c < 32) {
        ip.advance(1);
        c = static_cast<uint32_t>(get(ip.getLocation()));
    }

    tick();
}

void FungeWorld::tick() {
    InstructionPointer& ip = *pointers.front();
    pointers.pop();

    if(ip.getPointerState() == PointerState::STRING) {
        if(get(ip.getLocation()) == U'"') {
            ip.setPointerState(PointerState::NORMAL);
        } else {
            ip.getStack().push(get(ip.getLocation()));
        }
    } else {
        auto c = static_cast<uint32_t>(get(ip.getLocation()));

        Retry:
        while(c > 126 || c <= 32) {
            ip.advance(1);
            c = static_cast<uint32_t>(get(ip.getLocation()));

            if(!boundsCheck(ip)) {
                goto Out;
            }
        }

        if(c == U';') {
            ip.advance(1);
            c = static_cast<uint32_t>(get(ip.getLocation()));
            while(c != U';') {
                ip.advance(1);
                c = static_cast<uint32_t>(get(ip.getLocation()));

                if(!boundsCheck(ip)) {
                    goto Out;
                }
            }

            ip.advance(1);
            c = static_cast<uint32_t>(get(ip.getLocation()));
            goto Retry;
        }

        if(InstructionSet::supports(c)) {
            InstructionSet::execute(ip);
        } else {
            ip.setDelta(ip.getDelta() * -1);
        }
    }

    Out:
    if(ip.getPointerState() == PointerState::EXITING) {
        delete &ip;
    } else {
        ip.advance(1);

        if(!boundsCheck(ip)) {
            delete &ip;
        } else {
            pointers.push(&ip);
        }
    }

    if(!pointers.empty()) {
        // ReSharper disable once CppDFAInfiniteRecursion
        // If there is infinite recursion then there is an error in the B98 file. Under normal circumstances the IP will
        // hit a '@' or escape the boundaries.
        tick();
    } else {
        quit(0);
    }
}

FungeWorld::~FungeWorld() {
    while(!pointers.empty()) {
        const InstructionPointer* ip = pointers.front();
        pointers.pop();
        delete ip;
    }
}
