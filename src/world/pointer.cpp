#include "pointer.hpp"

#include "vector.hpp"

InstructionPointer::InstructionPointer(const int8_t dimensions):
        InstructionPointer(Vector::origin(dimensions), Vector::east(dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location):
        InstructionPointer(location, Vector::east(location.dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta):
        location(location), delta(delta), storageOffset(Vector::origin(location.dimensions)), stack(new Stack()), mode(PointerState::NORMAL) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset, const Stack* stack):
        location(location), delta(delta), storageOffset(offset), stack(new Stack(*stack)), mode(PointerState::NORMAL) {}

InstructionPointer* InstructionPointer::split() const {
    return new InstructionPointer(location, -delta, storageOffset, stack);
}

void InstructionPointer::advance(const int steps) {
    location += (delta * steps);
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

InstructionPointer::~InstructionPointer() {
    delete stack;
}
