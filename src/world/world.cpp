#include "world.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <limits>

std::unordered_map<char32_t, std::function<bool(InstructionPointer&)>> InstructionSet::commands;
FungeWorld* InstructionSet::world = nullptr;

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
        int32_t n;
        while(!(std::cin >> n)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
        std::cout << static_cast<char>(ip.getStack().popChar());
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
        std::cout << ip.getStack().pop() << " ";
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

    commands[U'>'] = [](InstructionPointer& ip) {
        ip.setDelta(1, 0, 0);
        return true;
    };

    commands[U'?'] = [](InstructionPointer& ip) {
        // ReSharper disable once CppDFANullDereference
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
            std::cout << "Entered" << std::endl;
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

    // TODO: implement sysinfo command
    commands[U'y'] = [](InstructionPointer& _) {
        return false;
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
        char c;
        std::cin.get(c);
        ip.getStack().push(static_cast<char32_t>(c));
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
            data.back().emplace_back(line.begin(), line.end());

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
        data.back().emplace_back(result.begin(), result.end());
        return new FungeWorld(data, Vector::origin(1), Vector(static_cast<int32_t>(result.size() - 1)));
    }

    if(dim == 2) {
        size_t maxRow = 0;
        data.emplace_back();
        std::string line;
        while(std::getline(file, line)) {
            data.back().emplace_back(line.begin(), line.end());

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
            data.back().emplace_back(line.begin(), line.end());

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
        yStarts[z] = location.getY();
        xStarts[z].insert(xStarts[z].begin(), yStarts[z] - location.getY(), 0);

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