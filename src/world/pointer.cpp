#include "pointer.hpp"

#include <cassert>
#include <ranges>

#include "../fingerprints/fingerprint.hpp"
#include "stack.hpp"
#include "vector.hpp"

uint32_t InstructionPointer::nextId = 0;

InstructionPointer::InstructionPointer(const int32_t dimensions):
        InstructionPointer(Vector::origin(dimensions), Vector::east(dimensions), Vector::origin(dimensions), std::make_unique<Stack>()) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset, std::unique_ptr<Stack> stack):
        location(location), delta(delta), storageOffset(offset), reciprocalDelta({}), stack(std::move(stack)), mode(PointerState::NORMAL), id(nextId++) {

    // Pass 1e-20 instead of zero to prevent NaN issues later.
    reciprocalDelta[0] = 1.0F / (delta.getX() == 0 ? 1e-20F : delta.getX());
    reciprocalDelta[1] = 1.0F / (delta.getY() == 0 ? 1e-20F : delta.getY());
    reciprocalDelta[2] = 1.0F / (delta.getZ() == 0 ? 1e-20F : delta.getZ());
}

std::unique_ptr<InstructionPointer> InstructionPointer::split() const {
    return std::make_unique<InstructionPointer>(location, -delta, storageOffset, std::make_unique<Stack>(*stack));
}

void InstructionPointer::advance(const int steps) {
    location += delta * steps;
}

bool InstructionPointer::loadFingerprint(const int32_t fingerprint) {
    const auto it = std::ranges::find(loadedFingerprints, fingerprint, &std::pair<int, std::shared_ptr<Fingerprint>>::first);
    std::shared_ptr<Fingerprint> fp;

    if(it != loadedFingerprints.end()) {
        fp = it->second;
    } else {
        fp = Fingerprint::load(fingerprint);
        if(!fp) {
            return false;
        }

        loadedFingerprints.emplace_back(fingerprint, fp);
    }

    for(const char32_t c : fp->getImplementedInstructions()) {
        activeFingerprints[c - U'A'].push_back(fp);
    }

    return true;
}

bool InstructionPointer::unloadFingerprint(const int32_t fingerprint) {
    const auto it = std::ranges::find(loadedFingerprints, fingerprint, &std::pair<int, std::shared_ptr<Fingerprint>>::first);
    std::shared_ptr<Fingerprint> fp;

    if(it != loadedFingerprints.end()) {
        fp = it->second;
    } else {
        fp = Fingerprint::load(fingerprint);
        if(!fp) {
            return false;
        }
    }

    for(const char32_t c : fp->getImplementedInstructions()) {
        std::vector<std::weak_ptr<Fingerprint>>& semantics = activeFingerprints.at(c - U'A');

        const auto inst = std::find_if(semantics.rbegin(), semantics.rend(), [&fp](const std::weak_ptr<Fingerprint>& w_ptr) {
            const auto locked = w_ptr.lock();
            return locked && locked == fp;
        });

        if(inst != semantics.rend()) {
            semantics.erase(std::next(inst).base());
        }
    }

    return true;
}

bool InstructionPointer::execute(const char32_t instruction) {
    assert(instruction >= U'A' && instruction <= U'Z');

    auto& semantic = activeFingerprints[instruction - U'A'];

    if(semantic.empty()) {
        return false;
    }

    std::erase_if(semantic, [](const std::weak_ptr<Fingerprint>& wp) {
        return wp.expired();
    });

    const std::weak_ptr<Fingerprint> fp = semantic[semantic.size() - 1];

    if(const std::shared_ptr<Fingerprint> fp_locked = fp.lock()) {
        return fp_locked->execute(instruction, *this);
    }

    return false;
}

const Vector& InstructionPointer::getLocation() const {
    return location;
}

const Vector& InstructionPointer::getDelta() const {
    return delta;
}

const std::array<float, 3>& InstructionPointer::getReciprocalDelta() const {
    return reciprocalDelta;
}

const Vector& InstructionPointer::getOffset() const {
    return storageOffset;
}

void InstructionPointer::setLocation(const Vector& v) {
    location = v;
}

void InstructionPointer::setLocation(int32_t x, int32_t y, int32_t z) {
    location = {x, y, z};
}

void InstructionPointer::setDelta(const Vector& v) {
    delta = v;

    reciprocalDelta[0] = 1.0F / (delta.getX() == 0 ? 1e-20F : delta.getX());
    reciprocalDelta[1] = 1.0F / (delta.getY() == 0 ? 1e-20F : delta.getY());
    reciprocalDelta[2] = 1.0F / (delta.getZ() == 0 ? 1e-20F : delta.getZ());
}

void InstructionPointer::setDelta(int32_t x, int32_t y, int32_t z) {
    delta = {x, y, z};

    reciprocalDelta[0] = 1.0F / (delta.getX() == 0 ? 1e-20F : delta.getX());
    reciprocalDelta[1] = 1.0F / (delta.getY() == 0 ? 1e-20F : delta.getY());
    reciprocalDelta[2] = 1.0F / (delta.getZ() == 0 ? 1e-20F : delta.getZ());
}

void InstructionPointer::setPointerState(const PointerState ipMode) {
    mode = ipMode;
}

PointerState InstructionPointer::getPointerState() const {
    return mode;
}

Stack& InstructionPointer::getStack() const {
    return *stack;
}

std::vector<int32_t> InstructionPointer::stackSizes() const {
    std::vector<int32_t> sizes;

    for(const std::stack<int32_t>& s : stack->ss) {
        sizes.push_back(static_cast<int32_t>(s.size()));
    }

    return sizes;
}

void InstructionPointer::startBlock() {
    const int32_t n = stack->pop();
    stack->split(n);

    switch(storageOffset.dimensions) {
        case 1:
            stack->soss()->push(storageOffset.getX());
            break;
        case 2:
            stack->soss()->push(storageOffset.getX());
            stack->soss()->push(storageOffset.getY());
            break;
        default:
            stack->soss()->push(storageOffset.getX());
            stack->soss()->push(storageOffset.getY());
            stack->soss()->push(storageOffset.getZ());
    }

    storageOffset = location + delta;
}

void InstructionPointer::endBlock() {
    assert(stack->soss() != nullptr);

    const int32_t n = stack->pop();
    int32_t x = 0, y = 0, z = 0;

    switch(storageOffset.dimensions) {
        case 1:
            x = stack->popFromSoss();
            break;
        case 2:
            y = stack->popFromSoss();
            x = stack->popFromSoss();
            break;
        default:
            z = stack->popFromSoss();
            y = stack->popFromSoss();
            x = stack->popFromSoss();
    }

    storageOffset = {x, y, z};

    stack->collapse(n);
}