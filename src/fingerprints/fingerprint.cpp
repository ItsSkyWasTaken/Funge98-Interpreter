#include "fingerprint.hpp"

#include <cassert>
#include <utility>

#include "bool.hpp"
#include "strn.hpp"

void Fingerprint::load(std::shared_ptr<FungeWorld> w) {
    world = std::move(w);
    fingerprints.emplace(0x424F4F4C, &BooleanFP::instance);
    fingerprints.emplace(0x5354524E, &StringFP::instance);
}

std::shared_ptr<Fingerprint> Fingerprint::load(const int32_t id) {
    const auto it = fingerprints.find(id);
    if (it == fingerprints.end()) {
        return nullptr;
    }

    std::shared_ptr<Fingerprint> fp = it->second();
    return fp;
}


bool Fingerprint::execute(const char32_t instruction, InstructionPointer& ip) {
    assert(instruction >= U'A' && instruction <= U'Z');
    return instructions[instruction - U'A'](this, ip);
}

// All potentially overloaded instructions do nothing and return false by default; fingerprints implement instructions
// in their respective classes by extending Fingerprint.

bool Fingerprint::a(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::b(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::c(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::d(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::e(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::f(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::g(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::h(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::i(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::j(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::k(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::l(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::m(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::n(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::o(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::p(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::q(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::r(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::s(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::t(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::u(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::v(InstructionPointer& ip) {
    return false;
}
bool Fingerprint::w(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::x(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::y(InstructionPointer& ip) {
    return false;
}

bool Fingerprint::z(InstructionPointer& ip) {
    return false;
}

Fingerprint::~Fingerprint() = default;