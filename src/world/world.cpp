#include "world.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <string>

#include "instructions.hpp"
#include "../strings.hpp"

FungeWorld* FungeWorld::fromFile(std::ifstream& file) {
    std::vector<std::u32string> planes;
    std::string utf8Plane;
    while(std::getline(file, utf8Plane, '\f')) {
        planes.push_back(Strings::fromUtf8(utf8Plane));
    }

    std::unordered_map<Vector, std::u32string> chunks;

    // Trefunge
    if(planes.size() > 1) {
        int p = 0, l = 0;
        int32_t maxX = 0, maxY = 0;
        for(const auto& plane : planes) {
            std::basic_stringstream<char32_t> planeStream(plane);
            std::u32string line;

            while(Strings::getLine(planeStream, line)) {
                if(line == U"\\f") {
                    maxY = std::max(maxY, l + 1);
                    l = 0;
                    p++;
                    continue;
                }

                for(size_t i = 0; i < line.length(); i += 16) {
                    const Vector chunk = {static_cast<int32_t>(i / 16), l / 16, p / 16};
                    chunks.try_emplace(chunk, 4096, U' ');
                    chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, 16, line.substr(i, 16));
                }

                maxX = std::max(maxX, static_cast<int32_t>(line.length()));
                l++;
            }

            maxY = std::max(maxY, l + 1);
            l = 0;
            p++;
        }

        return new FungeWorld(std::move(chunks), Vector(maxX, maxY, p + 1));
    }

    std::u32string& plane = planes[0];
    std::vector<std::u32string> lines;
    std::basic_stringstream<char32_t> planeStream(plane);
    std::u32string lineRead;
    bool trefunge = false;
    while(Strings::getLine(planeStream, lineRead)) {
        if(lineRead == U"\\f") {
            trefunge = true;
        }

        lines.push_back(lineRead);
    }

    // Trefunge, but all form feeds are "\f" sequences instead of literal form feed characters
    if(trefunge) {
        int p = 0, l = 0;
        int32_t maxX = 0, maxY = 0;

        for(const auto& line : lines) {
            if(line == U"\\f") {
                maxY = std::max(maxY, l + 1);
                l = 0;
                p++;
                continue;
            }

            for(size_t i = 0; i < line.length(); i += 16) {
                const Vector chunk = {static_cast<int32_t>(i / 16), l / 16, p / 16};
                chunks.try_emplace(chunk, 4096, U' ');
                chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, 16, line.substr(i, 16));
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        return new FungeWorld(std::move(chunks), Vector(maxX, maxY, p + 1));
    }

    // Befunge
    if(lines.size() > 1) {
        int l = 0;
        int32_t maxX = 0;

        for(const auto& line : lines) {
            for(size_t i = 0; i < line.length(); i += 64) {
                const Vector chunk = {static_cast<int32_t>(i / 64), l / 64};
                chunks.try_emplace(chunk, 4096, U' ');
                chunks[chunk].replace(l % 64 * 64, 64, line.substr(i, 64));
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        return new FungeWorld(std::move(chunks), Vector(maxX, l + 1));
    }

    // Unefunge
    std::u32string& line = lines[0];
    for(size_t i = 0; i < line.length(); i += 4096) {
        const Vector chunk = {static_cast<int32_t>(i / 4096)};
        std::u32string chunkString = line.substr(i, 4096);
        chunks.insert_or_assign(chunk, chunkString.append(std::max(0, static_cast<int32_t>(chunkString.length() - 4096)), U' '));
    }

    return new FungeWorld(std::move(chunks), Vector(line.length()));
}

FungeWorld* FungeWorld::fromFile(std::ifstream& file, const int dim) {
    std::unordered_map<Vector, std::u32string> chunks;
    std::vector<std::u32string> planes;
    std::string planeString;
    while(std::getline(file, planeString, '\f')) {
        planes.push_back(Strings::fromUtf8(planeString));
    }

    // Unefunge
    if(dim == 1) {
        std::u32string program;
        for(const auto& plane : planes) {
            std::basic_stringstream<char32_t> stream(plane);
            std::u32string l;
            while(Strings::getLine(stream, l)) {
                if(l != U"\\f") {
                    program.append(l);
                }
            }
        }

        for(size_t i = 0; i < program.length(); i += 4096) {
            const Vector chunk = {static_cast<int32_t>(i / 4096)};
            std::u32string chunkString = program.substr(i, 4096);
            chunks.insert_or_assign(chunk, chunkString.append(std::max(0, static_cast<int32_t>(chunkString.length() - 4096)), U' '));
        }

        return new FungeWorld(std::move(chunks), Vector(program.length()));
    }

    // Befunge
    if(dim == 2) {
        std::vector<std::u32string> lines;
        for(const auto& plane : planes) {
            std::basic_stringstream<char32_t> stream(plane);
            std::u32string l;
            while(Strings::getLine(stream, l)) {
                if(l != U"\\f") {
                    lines.push_back(l);
                }
            }
        }

        int l = 0;
        int32_t maxX = 0;

        for(const auto& line : lines) {
            for(size_t i = 0; i < line.length(); i += 64) {
                const Vector chunk = {static_cast<int32_t>(i / 64), l / 64};
                chunks.try_emplace(chunk, 4096, U' ');
                chunks[chunk].replace(l % 64 * 64, 64, line.substr(i, 64));
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        return new FungeWorld(std::move(chunks), Vector(maxX, l + 1));
    }

    // Trefunge
    assert(dim == 3);
    int p = 0, l = 0;
    int32_t maxX = 0, maxY = 0;
    for(const auto& plane : planes) {
        std::basic_stringstream<char32_t> planeStream(plane);
        std::u32string line;

        while(Strings::getLine(planeStream, line)) {
            if(line == U"\\f") {
                maxY = std::max(maxY, l + 1);
                l = 0;
                p++;
                continue;
            }

            for(size_t i = 0; i < line.length(); i += 16) {
                const Vector chunk = {static_cast<int32_t>(i / 16), l / 16, p / 16};
                chunks.try_emplace(chunk, 4096, U' ');
                chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, 16, line.substr(i, 16));
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        maxY = std::max(maxY, l + 1);
        l = 0;
        p++;
    }

    return new FungeWorld(std::move(chunks), Vector(maxX, maxY, p + 1));
}

char32_t FungeWorld::get(const Vector& location) {
    if(location.dimensions > dimensions) {
        return U' ';
    }

    const auto [chunk, offset] = getSublocation(location);
    return chunk[offset];
}

void FungeWorld::put(const Vector& location, const char32_t value) {
    if(location.dimensions > dimensions) {
        return;
    }

    const auto [chunk, offset] = getSublocation(location);
    chunk[offset] = value;
}

std::pair<std::u32string&, int> FungeWorld::getSublocation(const Vector& v) {
    const int32_t x = v.getX(), y = v.getY(), z = v.getZ();
    const std::pair<Vector, uint32_t> p = [&] {
        switch(dimensions) {
            case 1:
                return std::make_pair(
                    Vector(x >= 0 ? x / 4096 : (x + 1) / 4096 - 1),
                    x >= 0 ? x % 4096 : 4096 + x % 4096
                );
            case 2:
                return std::make_pair(
                    Vector(x >= 0 ? x / 64 : (x + 1) / 64 - 1, y >= 0 ? y / 64 : (y + 1) / 64 - 1),
                    (y >= 0 ? y % 64 : 64 + y % 64) * 64 + (x >= 0 ? x % 64 : 64 + x % 64)
                );
            default:
                assert(dimensions == 3);

                return std::make_pair(
                    Vector(x >= 0 ? x / 16 : (x + 1) / 16 - 1, y >= 0 ? y / 16 : (y + 1) / 16 - 1, z >= 0 ? z / 16 : (z + 1) / 16 - 1),
                    (z >= 0 ? z % 16 : 16 + z % 16) * 256 + (y >= 0 ? y % 16 : 16 + y % 16) * 16 + (x >= 0 ? x % 16 : 16 + x % 16)
                );
        }
    }();

    chunks.try_emplace(p.first, 4096, U' ');
    return { chunks[p.first], p.second };
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
        if(const char32_t c = get(ip.getLocation()); c == U'"') {
            ip.setPointerState(PointerState::NORMAL);
        } else if(c != U' ' || ip.getStack().peek() != U' ') {
            ip.getStack().push(c);
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

FungeWorld::FungeWorld(std::unordered_map<Vector, std::u32string> chunks, const Vector& high):
        chunks(std::move(chunks)), dimensions(high.dimensions), low(Vector::origin(high.dimensions)), high(high), write(false), read(false), execute(false) {}

FungeWorld::~FungeWorld() {
    while(!pointers.empty()) {
        const InstructionPointer* ip = pointers.front();
        pointers.pop();
        delete ip;
    }
}