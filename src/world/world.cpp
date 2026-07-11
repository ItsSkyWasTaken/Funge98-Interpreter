#include "world.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "../strings.hpp"
#include "instructions.hpp"
#include "pointer.hpp"
#include "stack.hpp"

std::shared_ptr<FungeWorld> FungeWorld::fromFile(std::ifstream& file) {
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
                    const std::u32string sub = line.substr(i, 16);
                    chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, sub.size(), sub);
                }

                maxX = std::max(maxX, static_cast<int32_t>(line.length()));
                l++;
            }

            maxY = std::max(maxY, l);
            l = 0;
            p++;
        }

        return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(maxX - 1, maxY - 1, p - 1));
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
                maxY = std::max(maxY, l);
                l = 0;
                p++;
                continue;
            }

            for(size_t i = 0; i < line.length(); i += 16) {
                const Vector chunk = {static_cast<int32_t>(i / 16), l / 16, p / 16};
                chunks.try_emplace(chunk, 4096, U' ');
                const std::u32string sub = line.substr(i, 16);
                chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, sub.size(), sub);
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(maxX - 1, maxY - 1, p - 1));
    }

    // Befunge
    if(lines.size() > 1) {
        int l = 0;
        int32_t maxX = 0;

        for(const auto& line : lines) {
            for(size_t i = 0; i < line.length(); i += 64) {
                const Vector chunk = {static_cast<int32_t>(i / 64), l / 64};
                chunks.try_emplace(chunk, 4096, U' ');
                const std::u32string sub = line.substr(i, 64);
                chunks[chunk].replace(l % 64 * 64, sub.size(), sub);
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(maxX - 1, l));
    }

    // Unefunge
    std::u32string& line = lines[0];
    for(size_t i = 0; i < line.length(); i += 4096) {
        const Vector chunk = {static_cast<int32_t>(i / 4096)};
        std::u32string chunkString = line.substr(i, 4096);
        chunks.insert_or_assign(chunk, chunkString.append(std::max(0, static_cast<int32_t>(chunkString.length() - 4096)), U' '));
    }

    return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(line.length() - 1));
}

std::shared_ptr<FungeWorld> FungeWorld::fromFile(std::ifstream& file, const int dim) {
    std::unordered_map<Vector, std::u32string> chunks;

    // Unefunge
    if(dim == 1) {
        std::u32string program;
        std::string line;
        while(Strings::getLine(file, line)) {
            program.append(Strings::fromUtf8(line));
        }

        for(size_t i = 0; i < program.length(); i += 4096) {
            const Vector chunk = {static_cast<int32_t>(i / 4096)};
            std::u32string chunkString = program.substr(i, 4096);
            chunks.insert_or_assign(chunk, chunkString.append(std::max(0, static_cast<int32_t>(chunkString.length() - 4096)), U' '));
        }

        return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(program.length() - 1));
    }

    // Befunge
    if(dim == 2) {
        std::vector<std::u32string> lines;
        std::string line;

        int l = 0;
        int32_t maxX = 0;
        while(Strings::getLine(file, line)) {
            const std::u32string line32 = Strings::fromUtf8(line);
            for(size_t i = 0; i < line.length(); i += 64) {
                const Vector chunk = {static_cast<int32_t>(i / 64), l / 64};
                chunks.try_emplace(chunk, 4096, U' ');
                const std::u32string sub = line32.substr(i, 64);
                chunks[chunk].replace(l % 64 * 64, sub.size(), sub);
            }

            maxX = std::max(maxX, static_cast<int32_t>(line32.length()));
            l++;
        }

        return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(maxX - 1, l - 1));
    }

    // Trefunge
    assert(dim == 3);
    std::vector<std::u32string> planes;
    std::string planeString;
    while(std::getline(file, planeString, '\f')) {
        planes.push_back(Strings::fromUtf8(planeString));
    }

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
                const std::u32string sub = line.substr(i, 16);
                chunks[chunk].replace(p % 16 * 256 + l % 16 * 16, sub.size(), sub);
            }

            maxX = std::max(maxX, static_cast<int32_t>(line.length()));
            l++;
        }

        maxY = std::max(maxY, l);
        l = 0;
        p++;
    }

    return std::make_shared<FungeWorld>(FungeToken{}, std::move(chunks), Vector(maxX - 1, maxY - 1, p - 1));
}

char32_t FungeWorld::get(const Vector& location) {
    if(location.dimensions > dimensions) {
        return U' ';
    }

    const auto [chunk, offset] = getSublocation(location);
    if(!chunk) {
        return U' ';
    }

    return (*chunk)[offset];
}

void FungeWorld::put(const Vector& location, const char32_t value) {
    if(location.dimensions > dimensions) {
        return;
    }

    const auto [chunk, offset] = getSublocation(location, value != U' ');
    if(!chunk) {
        return;
    }

    (*chunk)[offset] = value;

    if(value == U' ') {
        switch(dimensions) {
            case 1: {
                if(location.getX() == low.getX()) {
                    while(get(low) == U' ') {
                        low += {1};

                        if(low.getX() > high.getX()) {
                            quit(0);
                        }
                    }
                } else if(location.getX() == high.getX()) {
                    while(get(high) == U' ') {
                        low -= {1};
                    }
                }

                break;
            }
            case 2: {
                if(location.getX() == low.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t y = low.getY(); y <= high.getY(); y++) {
                            if(get({low.getX(), y}) != U' ') {
                                empty = false;
                                break;
                            }
                        }

                        if(empty) {
                            low += {1, 0};

                            if(low.getX() > high.getX()) {
                                quit(0);
                            }
                        }
                    }
                } else if(location.getX() == high.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t y = low.getY(); y <= high.getY(); y++) {
                            if(get({high.getX(), y}) != U' ') {
                                empty = false;
                                break;
                            }
                        }

                        if(empty) {
                            high -= {1, 0};
                        }
                    }
                }

                if(location.getY() == low.getY()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t x = low.getX(); x <= high.getX(); x++) {
                            if(get({x, low.getY()}) != U' ') {
                                empty = false;
                                break;
                            }
                        }

                        if(empty) {
                            low += {0, 1};

                            if(low.getY() > high.getY()) {
                                quit(0);
                            }
                        }
                    }
                } else if(location.getY() == high.getY()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t x = low.getX(); x <= high.getX(); x++) {
                            if(get({x, high.getY()}) != U' ') {
                                empty = false;
                                break;
                            }
                        }

                        if(empty) {
                            high -= {0, 1};
                        }
                    }
                }

                break;
            }
            default: {
                assert(dimensions == 3);

                if(location.getX() == low.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t z = low.getZ(); z <= high.getZ(); z++) {
                            for(int32_t y = low.getY(); y <= high.getY(); y++) {
                                if(get({low.getX(), y, z}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            low += {1, 0, 0};

                            if(low.getX() > high.getX()) {
                                quit(0);
                            }
                        }
                    }
                } else if(location.getX() == high.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t z = low.getZ(); z <= high.getZ(); z++) {
                            for(int32_t y = low.getY(); y <= high.getY(); y++) {
                                if(get({high.getX(), y, z}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            high -= {1, 0, 0};
                        }
                    }
                }

                if(location.getY() == low.getY()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t z = low.getZ(); z <= high.getZ(); z++) {
                            for(int32_t x = low.getX(); x <= high.getX(); x++) {
                                if(get({x, low.getY(), z}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            low += {0, 1, 0};

                            if(low.getY() > high.getY()) {
                                quit(0);
                            }
                        }
                    }
                } else if(location.getX() == high.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t z = low.getZ(); z <= high.getZ(); z++) {
                            for(int32_t x = low.getX(); x <= high.getX(); x++) {
                                if(get({x, high.getY(), z}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            high -= {0, 1, 0};
                        }
                    }
                }

                if(location.getZ() == low.getZ()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t y = low.getY(); y <= high.getY(); y++) {
                            for(int32_t x = low.getX(); x <= high.getX(); x++) {
                                if(get({x, y, low.getZ()}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            low += {0, 0, 1};

                            if(low.getZ() > high.getZ()) {
                                quit(0);
                            }
                        }
                    }
                } else if(location.getX() == high.getX()) {
                    bool empty = true;
                    while(empty) {
                        for(int32_t y = low.getY(); y <= high.getY(); y++) {
                            for(int32_t x = low.getX(); x <= high.getX(); x++) {
                                if(get({x, y, high.getZ()}) != U' ') {
                                    empty = false;
                                    break;
                                }
                            }

                            if(!empty) {
                                break;
                            }
                        }

                        if(empty) {
                            high -= {0, 0, 1};
                        }
                    }
                }
            }
        }
    } else {
        low = {std::min(low.getX(), location.getX()), std::min(low.getY(), location.getY()), std::min(low.getZ(), location.getZ())};
        high = {std::max(high.getX(), location.getX()), std::max(high.getY(), location.getY()), std::max(high.getZ(), location.getZ())};
    }
}

std::pair<std::u32string*, int> FungeWorld::getSublocation(const Vector& v, const bool generate) {
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

    assert(p.second < 4096);
    auto it = chunks.find(p.first);

    if(!generate) {
        if(it != chunks.end()) {
            return {&it->second, p.second};
        }

        return {nullptr, p.second};
    }

    if(it == chunks.end()) {
        auto [inserted_it, _] = chunks.try_emplace(p.first, 4096, U' ');
        it = inserted_it;
    }

    return {&it->second, p.second};
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

std::pair<const Vector&, const Vector&> FungeWorld::getBounds() const {
    return {low, high};
}

std::pair<bool, bool> FungeWorld::rebound(InstructionPointer& ip) const {
    const std::array<float, 3>& deltaInverse = ip.getReciprocalDelta();

    const float tx1 = (low.getX() - ip.getLocation().getX()) * deltaInverse[0];
    const float tx2 = (high.getX() - ip.getLocation().getX()) * deltaInverse[0];
    float tMin = std::min(tx1, tx2);
    float tMax = std::max(tx1, tx2);

    if(dimensions >= 2) {
        const float ty1 = (low.getY() - ip.getLocation().getY()) * deltaInverse[1];
        const float ty2 = (high.getY() - ip.getLocation().getY()) * deltaInverse[1];
        tMin = std::max(tMin, std::min(ty1, ty2));
        tMax = std::min(tMax, std::max(ty1, ty2));
    }

    if(dimensions >= 3) {
        const float tz1 = (low.getZ() - ip.getLocation().getZ()) * deltaInverse[2];
        const float tz2 = (high.getZ() - ip.getLocation().getZ()) * deltaInverse[2];
        tMin = std::max(tMin, std::min(tz1, tz2));
        tMax = std::min(tMax, std::max(tz1, tz2));
    }

    if(tMax < tMin) {
        return {false, false};
    }

    if(tMax < 0.0F) {
        const int32_t x = static_cast<int>(std::round(ip.getLocation().getX() + tMin * ip.getDelta().getX()));
        const int32_t y = dimensions >= 2 ? static_cast<int>(std::round(ip.getLocation().getY() + tMin * ip.getDelta().getY())) : 0;
        const int32_t z = dimensions >= 3 ? static_cast<int>(std::round(ip.getLocation().getZ() + tMin * ip.getDelta().getZ())) : 0;
        ip.setLocation(x, y, z);
        ip.advance(-1);

        return {true, true};
    }

    return {true, false};
}

std::pair<bool, bool> FungeWorld::rebound(Vector& location, const Vector& delta, const std::array<float, 3>& inverseDelta) const {
    const float tx1 = (low.getX() - location.getX()) * inverseDelta[0];
    const float tx2 = (high.getX() - location.getX()) * inverseDelta[0];
    float tMin = std::min(tx1, tx2);
    float tMax = std::max(tx1, tx2);

    if(dimensions >= 2) {
        const float ty1 = (low.getY() - location.getY()) * inverseDelta[1];
        const float ty2 = (high.getY() - location.getY()) * inverseDelta[1];
        tMin = std::max(tMin, std::min(ty1, ty2));
        tMax = std::min(tMax, std::max(ty1, ty2));
    }

    if(dimensions >= 3) {
        const float tz1 = (low.getZ() - location.getZ()) * inverseDelta[2];
        const float tz2 = (high.getZ() - location.getZ()) * inverseDelta[2];
        tMin = std::max(tMin, std::min(tz1, tz2));
        tMax = std::min(tMax, std::max(tz1, tz2));
    }

    if(tMax < tMin) {
        return {false, false};
    }

    if(tMax < 0.0F) {
        const int32_t x = static_cast<int>(std::round(location.getX() + tMin * delta.getX()));
        const int32_t y = dimensions >= 2 ? static_cast<int>(std::round(location.getY() + tMin * delta.getY())) : 0;
        const int32_t z = dimensions >= 3 ? static_cast<int>(std::round(location.getZ() + tMin * delta.getZ())) : 0;
        location = {x, y, z};

        return {true, true};
    }

    return {true, false};
}

void FungeWorld::quit(int code) {
    exitCode = code;
}

int FungeWorld::run() {
    pointers.push(std::make_unique<InstructionPointer>(dimensions));

    while(!pointers.empty() && !exitCode) {
        auto ip = std::move(pointers.front());
        pointers.pop();
        tick(std::move(ip));
    }

    return exitCode.value_or(0);
}

void FungeWorld::tick(std::unique_ptr<InstructionPointer> ip_ptr) {
    InstructionPointer& ip = *ip_ptr;
    if(ip.getPointerState() == PointerState::STRING) {
        if(const char32_t c = get(ip.getLocation()); c == U'"') {
            ip.setPointerState(PointerState::NORMAL);
        } else if(c == U' ') {
            ip.getStack().push(c);

            char32_t ch = static_cast<uint32_t>(get(ip.getLocation()));
            while(ch == U' ') {
                ip.advance(1);
                ch = static_cast<uint32_t>(get(ip.getLocation()));

                rebound(ip);
            }

            ip.setDelta(ip.getDelta() * -1);
            ip.advance(1);
            rebound(ip);
            ip.setDelta(ip.getDelta() * -1);
        } else {
            ip.getStack().push(c);
        }
    } else {
        auto c = static_cast<uint32_t>(get(ip.getLocation()));

        Retry:
        while(c > 126 || c <= 32) {
            ip.advance(1);
            c = static_cast<uint32_t>(get(ip.getLocation()));
            rebound(ip);
        }

        if(c == U';') {
            ip.advance(1);
            c = static_cast<uint32_t>(get(ip.getLocation()));
            while(c != U';') {
                ip.advance(1);
                c = static_cast<uint32_t>(get(ip.getLocation()));
                rebound(ip);
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
    if(ip.getPointerState() != PointerState::EXITING) {
        ip.advance(1);

        if(rebound(ip).second) {
            ip.advance(1);
        }

        pointers.push(std::move(ip_ptr));
    }
}

FungeWorld::FungeWorld(FungeToken, std::unordered_map<Vector, std::u32string> chunks, const Vector& size):
        chunks(std::move(chunks)), dimensions(size.dimensions), low(Vector::origin(size.dimensions)), high(size), write(false), read(false), execute(false) {}

FungeWorld::~FungeWorld() {
    while(!pointers.empty()) {
        pointers.pop();
    }
}