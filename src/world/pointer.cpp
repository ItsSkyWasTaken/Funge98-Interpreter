#include "pointer.hpp"

#include <cassert>

#include "vector.hpp"

uint32_t InstructionPointer::nextId = 0;

InstructionPointer::InstructionPointer(const int32_t dimensions):
        InstructionPointer(Vector::origin(dimensions), Vector::east(dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location):
        InstructionPointer(location, Vector::east(location.dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta):
        location(location), delta(delta), storageOffset(Vector::origin(location.dimensions)), stack(new Stack()), mode(PointerState::NORMAL), id(nextId++) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset, const Stack* stack):
        location(location), delta(delta), storageOffset(offset), stack(new Stack(*stack)), mode(PointerState::NORMAL), id(nextId++) {}

InstructionPointer* InstructionPointer::split() const {
    return new InstructionPointer(location, -delta, storageOffset, stack);
}

void InstructionPointer::advance(const int steps) {
    location += delta * steps;
}

const Vector& InstructionPointer::getLocation() const {
    return location;
}

const Vector& InstructionPointer::getDelta() const {
    return delta;
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
}

void InstructionPointer::setDelta(int32_t x, int32_t y, int32_t z) {
    delta = {x, y, z};
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

InstructionPointer::~InstructionPointer() {
    delete stack;
}
