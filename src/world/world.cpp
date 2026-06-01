#include "world.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <string>

#include "instructions.hpp"

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

void FungeWorld::run() {
    pointers.push(new InstructionPointer(dimensions));

    while(!pointers.empty()) {
        InstructionPointer& ip = *pointers.front();
        pointers.pop();
        tick(ip);
    }

    quit(0);
}



void FungeWorld::tick(InstructionPointer& ip) {
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
}

FungeWorld::~FungeWorld() {
    while(!pointers.empty()) {
        const InstructionPointer* ip = pointers.front();
        pointers.pop();
        delete ip;
    }
}