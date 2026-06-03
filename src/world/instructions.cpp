#include "instructions.hpp"

#include <cassert>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>

#include "../strings.hpp"

std::unordered_map<char32_t, std::function<bool(InstructionPointer&)>> InstructionSet::commands;
FungeWorld* InstructionSet::world = nullptr;
int InstructionSet::pointerPosition = 0;
int InstructionSet::ansiFlag = 0;
int InstructionSet::ansiParameter = 0;

void InstructionSet::load(FungeWorld& w) {
    world = &w;
    commands.reserve(91);

    // Perform boolean inversion on the TOS.
    commands[U'!'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        stack.push(!stack.pop());
        return true;
    };

    // Toggle String Mode.
    commands[U'"'] = [](InstructionPointer& ip) {
        ip.setPointerState(PointerState::STRING);
        return true;
    };

    // Skip next Funge space cell.
    commands[U'#'] = [](InstructionPointer& ip) {
        ip.advance(1);
        return true;
    };

    // Pop top of the stack.
    commands[U'$'] = [](const InstructionPointer& ip) {
        ip.getStack().pop();
        return true;
    };

    // Perform modulus operation on top two elements.
    commands[U'%'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();

        // Return 0 if divisor is 0, instead of erroring.
        stack.push(b == 0 ? 0 : a % b);

        return true;
    };

    // Read integer from input stream.
    commands[U'&'] = [](const InstructionPointer& ip) {
        std::string input;
        int32_t n;

        getline(std::cin, input);
        std::istringstream iss(input);

        // Attempt to grab integer from input. If it fails...
        while(!(iss >> n)) {
            // Flush input stream.
            std::cin.clear();

            // Delete line in terminal and reset cursor.
            std::cout << "\033[1F\033[" << pointerPosition << "C\033[0J";

            // Try again.
            getline(std::cin, input);
            iss = std::istringstream(input);
        }

        ip.getStack().push(n);
        return true;
    };

    // Push the character at the IP's next location.
    commands[U'\''] = [](InstructionPointer& ip) {
        ip.advance(1);
        ip.getStack().push(world->get(ip.getLocation()));
        return true;
    };

    // Perform multiplication on the top two elements of the stack.
    commands[U'*'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when multiplying.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast product back to a signed integer.
        stack.push(static_cast<int32_t>(a * b));
        return true;
    };

    // Perform addition on the top two elements of the stack.
    commands[U'+'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when adding.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast sum back to a signed integer.
        stack.push(static_cast<int32_t>(a + b));
        return true;
    };

    // Print a Unicode representation of the top value of the stack.
    commands[U','] = [](const InstructionPointer& ip) {
        const char32_t c = ip.getStack().popChar();
        const std::u32string s(1, c);
        std::cout << Strings::toUtf8(s);

        // Check ANSI status, to determine how to update the cursor.
        switch(ansiFlag) {
            // If an ANSI code is active:
            case 2:
                // These codepoints end an ANSI code.
                if(c >= 64 && c < 127) {
                    ansiFlag = 0;

                    // Update cursor position according to which ANSI code was used.
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
                            // Nothing special; do nothing.
                            break;
                    }

                    ansiParameter = 0;

                // These codepoints are digits, which are parameters; update parameter tracker accordingly.
                } else if(c >= 48 && c < 58) {
                    ansiParameter *= 10;
                    ansiParameter += static_cast<int>(c - '0');
                }
                break;

            // If an escape character was passed, check for the '[' that's supposed to come next.
            case 1:
                if(c == U'[') {
                    ansiFlag = 2;
                    return true;
                }

                ansiFlag = 0;
                // Intentional fallthrough

            // Update cursor position accordingly; check for newlines and other special characters.
            default:
                // Newlines.
                if(c > 9 && c < 13) {
                    pointerPosition = 0;

                // Backspace.
                } else if(c == 8) {
                    if(pointerPosition > 0) {
                        pointerPosition -= 1;
                    }

                // Tab.
                } else if(c == 9) {
                    const int space = 8 - pointerPosition % 8;
                    pointerPosition += space == 0 ? 8 : space;

                // Escape character.
                } else if(c == 27) {
                    ansiFlag = 1;

                // All printable characters.
                } else if(c >= 32 && c != 127) {
                    pointerPosition += 1;
                }
        }

        return true;
    };

    // Perform subtraction on the top two values of the stack.
    commands[U'-'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

        // Cast values to unsigned integers to guarantee wrap-around when subtracting.
        const auto b = static_cast<uint32_t>(stack.pop()), a = static_cast<uint32_t>(stack.pop());

        // Cast difference back to a signed integer.
        stack.push(static_cast<int32_t>(a - b));
        return true;
    };

    // Print the top integer of the stack, followed by a space.
    commands[U'.'] = [](const InstructionPointer& ip) {
        const int32_t n = ip.getStack().pop();
        std::cout << n;

        // Check for ANSI status.
        switch(ansiFlag) {
            // If an ANSI code is active, update parameter for tracking the cursor. Do NOT print a space (it will mess
            // up the ANSI code).
            case 2:
                ansiParameter *= 10 * (n == 0 ? 1 : static_cast<int>(log10(abs(n))) + 1);
                ansiParameter += n;
                break;

            // If the escape character was passed but the '[' was not, clear ANSI flag.
            case 1:
                ansiFlag = 0;
                // Intentional fallthrough.

            // Count digits in number to track cursor, and DO print the space.
            default:
                pointerPosition += n == 0 ? 2 : static_cast<int>(log10(abs(n))) + 2 + (n < 0 ? 1 : 0);
                std::cout << " ";
        }

        return true;
    };

    // Perform division on the top two elements of the stack.
    commands[U'/'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();

        // If the divisor is 0, return 0 instead of erroring.
        stack.push(b == 0 ? 0 : a / b);

        return true;
    };

    // Push 0.
    commands[U'0'] = [](const InstructionPointer& ip) {
        ip.getStack().push(0);
        return true;
    };

    // Push 1.
    commands[U'1'] = [](const InstructionPointer& ip) {
        ip.getStack().push(1);
        return true;
    };

    // Push 2.
    commands[U'2'] = [](const InstructionPointer& ip) {
        ip.getStack().push(2);
        return true;
    };

    // Push 3.
    commands[U'3'] = [](const InstructionPointer& ip) {
        ip.getStack().push(3);
        return true;
    };

    // Push 4.
    commands[U'4'] = [](const InstructionPointer& ip) {
        ip.getStack().push(4);
        return true;
    };

    // Push 5.
    commands[U'5'] = [](const InstructionPointer& ip) {
        ip.getStack().push(5);
        return true;
    };

    // Push 6.
    commands[U'6'] = [](const InstructionPointer& ip) {
        ip.getStack().push(6);
        return true;
    };

    // Push 7.
    commands[U'7'] = [](const InstructionPointer& ip) {
        ip.getStack().push(7);
        return true;
    };

    // Push 8.
    commands[U'8'] = [](const InstructionPointer& ip) {
        ip.getStack().push(8);
        return true;
    };

    // Push 9.
    commands[U'9'] = [](const InstructionPointer& ip) {
        ip.getStack().push(9);
        return true;
    };

    // Duplicate top value of the stack.
    commands[U':'] = [](const InstructionPointer& ip) {
        ip.getStack().duplicate();
        return true;
    };

    // Go west.
    commands[U'<'] = [](InstructionPointer& ip) {
        ip.setDelta(-1, 0, 0);
        return true;
    };

    // Execute system command.
    commands[U'='] = [](const InstructionPointer& ip) {
        // Permission check.
        if(!world->canExecute()) {
            return false;
        }

        const char* command = Strings::toUtf8(ip.getStack().popString()).c_str();
        const int32_t status = std::system(command);
        ip.getStack().push(status);
        return true;
    };

    // Go east.
    commands[U'>'] = [](InstructionPointer& ip) {
        ip.setDelta(1, 0, 0);
        return true;
    };

    // Go in a random cardinal direction.
    commands[U'?'] = [](InstructionPointer& ip) {
        ip.setDelta(Vector::random(world->dimensions));
        return true;
    };

    // Halt instruction pointer.
    commands[U'@'] = [](InstructionPointer& ip) {
        ip.setPointerState(PointerState::EXITING);
        return true;
    };

    // Turn left.
    commands[U'['] = [](InstructionPointer& ip) {
        const Vector& delta = ip.getDelta();
        if(delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(delta.getY(), -delta.getX(), delta.getZ());
        return true;
    };

    // Swap top two elements.
    commands[U'\\'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(b);
        stack.push(a);
        return true;
    };

    // Turn right.
    commands[U']'] = [](InstructionPointer& ip) {
        const Vector& delta = ip.getDelta();
        if(delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(-delta.getY(), delta.getX(), delta.getZ());
        return true;
    };

    // Go north.
    commands[U'^'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, -1, 0);
        return true;
    };

    // Go west if TOS is true, go east if false.
    commands[U'_'] = [](InstructionPointer& ip) {
        ip.setDelta(ip.getStack().pop() ? -1 : 1, 0, 0);
        return true;
    };

    // Greater-than operation on top two elements.
    commands[U'`'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t b = stack.pop(), a = stack.pop();
        stack.push(a > b);
        return true;
    };

    // Push 10.
    commands[U'a'] = [](const InstructionPointer& ip) {
        ip.getStack().push(10);
        return true;
    };

    // Push 11.
    commands[U'b'] = [](const InstructionPointer& ip) {
        ip.getStack().push(11);
        return true;
    };

    // Push 12.
    commands[U'c'] = [](const InstructionPointer& ip) {
        ip.getStack().push(12);
        return true;
    };

    // Push 13.
    commands[U'd'] = [](const InstructionPointer& ip) {
        ip.getStack().push(13);
        return true;
    };

    // Push 14.
    commands[U'e'] = [](const InstructionPointer& ip) {
        ip.getStack().push(14);
        return true;
    };

    // Push 15.
    commands[U'f'] = [](const InstructionPointer& ip) {
        ip.getStack().push(15);
        return true;
    };

    // Get value at location in Funge world.
    commands[U'g'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        stack.push(world->get(ip.getOffset() + stack.popVector(world->dimensions)));
        return true;
    };

    // Go high.
    commands[U'h'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, -1);
        return true;
    };

    // Read file into Funge world
    commands[U'i'] = [](const InstructionPointer& ip) {
        if(!world->read) {
            return false;
        }

        Stack& stack = ip.getStack();

        const std::u32string filename = stack.popString();
        const int32_t bin = stack.pop();
        std::ifstream file(Strings::toUtf8(filename), std::ios::binary | std::ios::ate);

        if(!file.is_open()) {
            stack.push(bin);
            stack.push(filename);
            return false;
        }

        const std::streamsize fileSize = file.tellg();
        std::string buffer(fileSize, '\0');
        file.seekg(0, std::ios::beg);

        if(!file.read(buffer.data(), fileSize)) {
            stack.push(bin);
            stack.push(filename);
            return false;
        }

        file.close();
        std::u32string contents = Strings::fromUtf8(buffer);

        const Vector start = stack.popVector(world->dimensions);
        Vector cursor = start;

        // Binary mode (same behavior across all 3 sub-languages)
        if(bin & 1) {
            for(const auto& c : contents) {
                if(c != U' ') {
                    world->put(cursor, c);
                }

                cursor += {1, 1, 1};
            }

            const Vector upperBound = cursor + Vector(1, 0, 0), size = upperBound - start;
            world->low = {std::min(world->low.getX(), start.getX()), world->low.getY(), world->low.getZ()};
            world->high = {std::max(world->high.getX(), upperBound.getX()), world->high.getY(), world->high.getZ()};

            stack.push(start);
            stack.push(size);
            return true;
        }

        // Unefunge
        if(world->dimensions == 1) {
            std::basic_stringstream<char32_t> contentStream(contents);
            std::u32string line;
            while(Strings::getLine(contentStream, line)) {
                if(line == U"\\f") {
                    continue;
                }

                for(const auto& c : line) {
                    if(c != U' ') {
                        world->put(cursor, c);
                    }

                    cursor += {1};
                }
            }

            const Vector upperBound = cursor + Vector(1), size = upperBound - start;
            world->low = {std::min(world->low.getX(), start.getX())};
            world->high = {std::max(world->high.getX(), upperBound.getX())};

            stack.push(start);
            stack.push(size);
            return true;
        }

        // Befunge
        if(world->dimensions == 2) {
            std::basic_stringstream<char32_t> contentStream(contents);
            std::u32string plane;
            int32_t maxX = 0;

            while(std::getline(contentStream, plane, U'\f')) {
                std::basic_stringstream<char32_t> planeStream(plane);
                std::u32string line;

                while(Strings::getLine(planeStream, line)) {
                    if(line == U"\\f") {
                        continue;
                    }

                    for(const auto& c : line) {
                        if(c != U' ') {
                            world->put(cursor, c);
                        }

                        cursor += {1, 0};
                    }

                    maxX = std::max(maxX, cursor.getX() + 1);
                    cursor = {start.getX(), cursor.getY() + 1};
                }
            }

            const auto upperBound = Vector(maxX, cursor.getY()), size = upperBound - start;
            world->low = {std::min(world->low.getX(), start.getX()), std::min(world->low.getY(), start.getY())};
            world->high = {std::max(world->high.getX(), upperBound.getX()), std::max(world->high.getY(), upperBound.getY())};

            stack.push(start);
            stack.push(size);
            return true;
        }

        // Trefunge
        assert(world->dimensions == 3);
        std::basic_stringstream<char32_t> contentStream(contents);
        std::u32string plane;
        int32_t maxX = 0, maxY = 0;

        while(std::getline(contentStream, plane, U'\f')) {
            std::basic_stringstream<char32_t> planeStream(plane);
            std::u32string line;

            while(Strings::getLine(planeStream, line)) {
                if(line == U"\\f") {
                    maxX = std::max(maxX, cursor.getX());
                    maxY = std::max(maxY, cursor.getY());
                    cursor = {start.getX(), start.getY(), cursor.getZ() + 1};
                    continue;
                }

                for(const auto& c : line) {
                    if(c != U' ') {
                        world->put(cursor, c);
                    }

                    cursor += {1, 0, 0};
                }

                maxX = std::max(maxX, cursor.getX() + 1);
                cursor = {start.getX(), cursor.getY() + 1, cursor.getZ()};
            }

            maxX = std::max(maxX, cursor.getX() + 1);
            maxY = std::max(maxY, cursor.getY());
            cursor = {start.getX(), start.getY(), cursor.getZ() + 1};
        }

        const auto upperBound = Vector(maxX, maxY, cursor.getZ()), size = upperBound - start;
        world->low = {std::min(world->low.getX(), start.getX()), std::min(world->low.getY(), start.getY()), std::min(world->low.getZ(), start.getZ())};
        world->high = {std::max(world->high.getX(), upperBound.getX()), std::max(world->high.getY(), upperBound.getY()), std::max(world->high.getZ(), upperBound.getZ())};

        stack.push(start);
        stack.push(size);
        return true;
    };

    // Jump a number of cells.
    commands[U'j'] = [](InstructionPointer& ip) {
        ip.advance(ip.getStack().pop());
        return true;
    };

    // Iterate next instruction.
    commands[U'k'] = [](InstructionPointer& ip) {
        const Vector old = ip.getLocation();
        const int32_t n = ip.getStack().pop();

        // Negative parameters result in an error.
        if(n < 0) {
            return false;
        }

        // Find next instruction.
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

        // TODO: implement wrap-around
        ip.setLocation(old);
        return false;
    };

    // Go low.
    commands[U'l'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, 1);
        return true;
    };

    // High-low conditional.
    commands[U'm'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions != 3) {
            return false;
        }

        ip.setDelta(0, 0, ip.getStack().pop() ? -1 : 1);
        return true;
    };

    // Clear stack.
    commands[U'n'] = [](const InstructionPointer& ip) {
        ip.getStack().clear();
        return true;
    };

    // Output File
    commands[U'o'] = [](const InstructionPointer& ip) {
        if(!world->write) {
            return false;
        }

        Stack& stack = ip.getStack();

        const std::u32string u32filename = stack.popString();
        const std::filesystem::path filePath = Strings::toUtf8(u32filename), parentPath = filePath.parent_path();

        if(!parentPath.empty() && !std::filesystem::exists(parentPath)) {
            std::error_code ec;
            std::filesystem::create_directories(parentPath, ec);
            if(ec) {
                stack.push(u32filename);
                return false;
            }
        }

        std::ofstream outFile(filePath);
        if(!outFile.is_open()) {
            stack.push(u32filename);
            return false;
        }

        const int32_t flags = stack.pop();
        const Vector size = stack.popVector(world->dimensions);
        const Vector origin = stack.popVector(world->dimensions);

        std::u32string contents;

        switch(world->dimensions) {
            case 1:
                contents.reserve(size.getX());
                for(int x = 0; x < size.getX(); x++) {
                    char32_t c = world->get(origin + Vector(x));

                    if(flags & 1) {
                        if(c == U'\f') {
                            if(size_t p = contents.find_last_not_of(U" \n\r"); p != std::u32string::npos) {
                                contents.erase(p + 1);
                            } else {
                                contents.clear();
                            }
                        } else if(c == U'\n') {
                            if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                contents.erase(p + 1);
                            } else {
                                contents.clear();
                            }
                        } else if(c == U'\r') {
                            if(world->get(origin + Vector(x + 1)) == U'\n') {
                                x++;
                            }

                            if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                contents.erase(p + 1);
                            } else {
                                contents.clear();
                            }
                        }
                    }

                    contents.push_back(c);
                }

                if(flags & 1) {
                    if(size_t p = contents.find_last_not_of(U" \n\r\f"); p != std::u32string::npos) {
                        contents.erase(p + 1);
                    } else {
                        contents.clear();
                    }
                }

                break;
            case 2:
                contents.reserve(size.getX() * size.getY());
                for(int y = 0; y < size.getY(); y++) {
                    for(int x = 0; x < size.getX(); x++) {
                        char32_t c = world->get(origin + Vector(x, y));

                        if(flags & 1) {
                            if(c == U'\f') {
                                if(size_t p = contents.find_last_not_of(U" \n\r"); p != std::u32string::npos) {
                                    contents.erase(p + 1);
                                } else {
                                    contents.clear();
                                }
                            } else if(c == U'\n') {
                                if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                    contents.erase(p + 1);
                                } else {
                                    contents.clear();
                                }
                            } else if(c == U'\r') {
                                if(world->get(origin + Vector(x + 1)) == U'\n') {
                                    x++;
                                }

                                if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                    contents.erase(p + 1);
                                } else {
                                    contents.clear();
                                }
                            }
                        }

                        contents.push_back(c);
                    }

                    if(flags & 1) {
                        if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                            contents.erase(p + 1);
                        } else {
                            contents.clear();
                        }
                    }

                    contents.push_back(U'\n');
                }

                if(flags & 1) {
                    if(size_t p = contents.find_last_not_of(U" \n\r\f"); p != std::u32string::npos) {
                        contents.erase(p + 1);
                    } else {
                        contents.clear();
                    }
                }

                break;
            default:
                assert(world->dimensions == 3);
                contents.reserve(size.getX() * size.getY() * size.getZ());

                for(int z = 0; z < size.getZ(); z++) {
                    for(int y = 0; y < size.getY(); y++) {
                        for(int x = 0; x < size.getX(); x++) {
                            char32_t c = world->get(origin + Vector(x, y, z));

                            if(flags & 1) {
                                if(c == U'\f') {
                                    if(size_t p = contents.find_last_not_of(U" \n\r"); p != std::u32string::npos) {
                                        contents.erase(p + 1);
                                    } else {
                                        contents.clear();
                                    }
                                } else if(c == U'\n') {
                                    if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                        contents.erase(p + 1);
                                    } else {
                                        contents.clear();
                                    }
                                } else if(c == U'\r') {
                                    if(world->get(origin + Vector(x + 1)) == U'\n') {
                                        x++;
                                    }

                                    if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                        contents.erase(p + 1);
                                    } else {
                                        contents.clear();
                                    }
                                }
                            }

                            contents.push_back(c);
                        }

                        if(flags & 1) {
                            if(size_t p = contents.find_last_not_of(U" "); p != std::u32string::npos) {
                                contents.erase(p + 1);
                            } else {
                                contents.clear();
                            }
                        }

                        contents.push_back(U'\n');
                    }

                    if(flags & 1) {
                        if(size_t p = contents.find_last_not_of(U" \n\r"); flags & 1 && p != std::u32string::npos) {
                            contents.erase(p + 1);
                        } else {
                            contents.clear();
                        }
                    }

                    contents.push_back(U'\f');
                }

                if(flags & 1) {
                    if(size_t p = contents.find_last_not_of(U" \n\r\f"); p != std::u32string::npos) {
                        contents.erase(p + 1);
                    } else {
                        contents.clear();
                    }
                }
        }

        outFile << Strings::toUtf8(contents);
        outFile.close();
        return true;
    };

    // Put value of TOS at a location in the Funge space.
    commands[U'p'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const Vector& v = stack.popVector(world->dimensions);
        const int32_t n = stack.pop();
        world->put(ip.getOffset() + v, n);
        return true;
    };

    // Quit program.
    commands[U'q'] = [](const InstructionPointer& ip) {
        quit(ip.getStack().pop());
        return true;
    };

    // Reflect pointer.
    commands[U'r'] = [](InstructionPointer& ip) {
        ip.setDelta(ip.getDelta() * -1);
        return true;
    };

    // Store character.
    commands[U's'] = [](InstructionPointer& ip) {
        ip.advance(1);
        world->put(ip.getLocation(), ip.getStack().popChar());
        return true;
    };

    // Split pointer (concurrent Funge).
    commands[U't'] = [](const InstructionPointer& ip) {
        InstructionPointer* ip2 = ip.split();
        ip2->advance(1);
        world->pointers.push(ip2);
        return true;
    };

    // Access stack under stack.
    commands[U'u'] = [](const InstructionPointer& ip) {
        if(ip.getStack().size() < 2) {
            return false;
        }

        ip.getStack().transfer(ip.getStack().pop());
        return true;
    };

    // Go south.
    commands[U'v'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, 1, 0);
        return true;
    };

    // Compare and turn.
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

    // Set delta.
    commands[U'x'] = [](InstructionPointer& ip) {
        const Vector v = ip.getStack().popVector(ip.getDelta().dimensions);
        ip.setDelta(v);
        return true;
    };

    // Get SysInfo.
    commands[U'y'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();
        const int32_t c = stack.pop();
        const std::vector<int32_t> sizes = ip.stackSizes();

        switch(c) {
            // If out of range, push everything.
            default:

            // Case 20: push environment variables onto the stack as null-terminated strings (unordered); the end is
            // terminated by another null character.
            case 20: {
                stack.push(0);
                for(const std::u32string& envar : world->envars) {
                    stack.push(envar);
                }

                // Only break if the value was specified; else, fall through and push everything else.
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

            // Case 4: push version number - v0.2.1 = 201.
            case 4: {
                stack.push(200);
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
                bits |= world->canWrite() ? 4 : 0;
                bits |= world->canRead() ? 2 : 0;

                stack.push(bits);
            }
        }

        return true;
    };

    // No-op.
    commands[U'z'] = [](InstructionPointer& _) {
        return true;
    };

    // Begin scope.
    commands[U'{'] = [](InstructionPointer& ip) {
        ip.startBlock();
        return true;
    };

    // North-south conditional.
    commands[U'|'] = [](InstructionPointer& ip) {
        if(const Vector& delta = ip.getDelta(); delta.dimensions == 1) {
            return false;
        }

        ip.setDelta(0, ip.getStack().pop() ? -1 : 1, 0);
        return true;
    };

    // End scope.
    commands[U'}'] = [](InstructionPointer& ip) {
        if(ip.getStack().size() < 2) {
            return false;
        }

        ip.endBlock();
        return true;
    };

    // Get character from input stream.
    commands[U'~'] = [](const InstructionPointer& ip) {
        Stack& stack = ip.getStack();

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
    if(!commands[world->get(ip.getLocation())](ip)) {
        ip.setDelta(ip.getDelta() * -1);
        return false;
    }

    return true;
}