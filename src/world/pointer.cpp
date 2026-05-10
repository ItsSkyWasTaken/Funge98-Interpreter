#include "pointer.hpp"
#include "vector.hpp"

InstructionPointer::InstructionPointer(const int8_t dimensions):
    InstructionPointer(Vector::origin(dimensions), Vector::east(dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location):
    InstructionPointer(location, Vector::east(location.dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta):
    InstructionPointer(location, delta, Vector::origin(location.dimensions)) {}

InstructionPointer::InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset):
    location(location), delta(delta), storageOffset(offset), mode(PointerMode::NORMAL) {}

InstructionPointer* InstructionPointer::split() const {
    return new InstructionPointer(location, -delta, storageOffset);
}

void InstructionPointer::advance() {
    location += delta;
}

void InstructionPointer::setPointerMode(const PointerMode ipMode) {
    mode = ipMode;
}

PointerMode InstructionPointer::getPointerMode() const {
    return mode;
}