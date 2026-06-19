#include "bool.hpp"

#include "../world/pointer.hpp"
#include "../world/stack.hpp"

std::shared_ptr<BooleanFP> BooleanFP::instance() {
    if(!inst) {
        inst = std::make_shared<BooleanFP>(FingerprintToken{});
    }

    return inst;
}

BooleanFP::BooleanFP(const FingerprintToken token): Fingerprint(token, false) {
    implementedInstructions.insert(implementedInstructions.end(), {U'A', U'N', U'O', U'X'});
}

bool BooleanFP::a(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t b = stack.pop();
    const int32_t a = stack.pop();

    stack.push(a & b);
    return true;
}

bool BooleanFP::n(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    stack.push(~stack.pop());
    return true;
}

bool BooleanFP::o(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t b = stack.pop();
    const int32_t a = stack.pop();

    stack.push(a | b);
    return true;
}

bool BooleanFP::x(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t b = stack.pop();
    const int32_t a = stack.pop();

    stack.push(a ^ b);
    return true;
}
