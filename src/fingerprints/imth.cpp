#include "imth.hpp"

#include <cstdint>
#include <iostream>
#include <limits>

#include "../world/pointer.hpp"
#include "../world/stack.hpp"

std::shared_ptr<IMathFP> IMathFP::instance() {
    if(!inst) {
        inst = std::make_shared<IMathFP>(FingerprintToken{});
    }

    return inst;
}

IMathFP::IMathFP(const FingerprintToken token): Fingerprint(token, false) {
    implementedInstructions.insert(implementedInstructions.end(), {U'A', U'C', U'D', U'F', U'G', U'I', U'L', U'M', U'N', U'P', U'R', U'S', U'V'});
}

bool IMathFP::a(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    if(n < 0) {
        stack.push(n);
        return false;
    }

    if(n == 0) {
        stack.push(0);
        return true;
    }

    int32_t sum = 0;
    for(int i = 0; i < n; i++) {
        sum += stack.pop();
    }

    const int32_t avg = sum / n;
    stack.push(avg);
    return true;
}

bool IMathFP::b(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    stack.push(n == std::numeric_limits<int32_t>::min() ? n : std::abs(n));
    return true;
}

bool IMathFP::c(InstructionPointer& ip) {
    Stack& stack = ip.getStack();

    // Get as unsigned to protect against overflow.
    const auto n = static_cast<uint32_t>(stack.pop());

    stack.push(static_cast<int32_t>(n * 100));
    return true;
}

bool IMathFP::d(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    stack.push(n == 0 ? 0 : n < 0 ? n + 1 : n - 1);
    return true;
}

bool IMathFP::e(InstructionPointer& ip) {
    Stack& stack = ip.getStack();

    // Get as unsigned to protect against overflow.
    const auto n = static_cast<uint32_t>(stack.pop());

    stack.push(static_cast<int32_t>(n * 10000));
    return true;
}

bool IMathFP::f(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    if(n >= 34) {
        stack.push(0);
        return true;
    }

    if(n < 0) {
        stack.push(n);
        return false;
    }

    uint32_t result = 1;
    for(int i = 2; i <= n; i++) {
        result *= i;
    }

    stack.push(static_cast<int32_t>(result));
    return true;
}

bool IMathFP::g(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    stack.push(n == 0 ? 0 : n < 0 ? -1 : 1);
    return true;
}

bool IMathFP::h(InstructionPointer& ip) {
    Stack& stack = ip.getStack();

    // Get as unsigned to protect against overflow.
    const auto n = static_cast<uint32_t>(stack.pop());

    stack.push(static_cast<int32_t>(n * 1000));
    return true;
}

bool IMathFP::i(InstructionPointer& ip) {
    Stack& stack = ip.getStack();

    switch(const int32_t n = stack.pop()) {
        case std::numeric_limits<int32_t>::min():
            stack.push(std::numeric_limits<int32_t>::max());
            break;
        case std::numeric_limits<int32_t>::max():
            stack.push(std::numeric_limits<int32_t>::min());
            break;
        case 0:
            stack.push(0);
            break;
        default:
            stack.push(n > 0 ? n + 1 : n - 1);
    }

    return true;
}

bool IMathFP::l(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t c = stack.pop();
    const int32_t n = stack.pop();

    stack.push(std::abs(c) >= 32 ? 0 : c >= 0 ? n << c : n >> -c);
    return true;
}

bool IMathFP::n(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    if(n <= 0) {
        stack.push(n);
        return false;
    }

    int32_t min = std::numeric_limits<int32_t>::max();
    for(int i = 0; i < n; i++) {
        const int32_t val = stack.pop();
        min = std::min(min, val);
    }

    stack.push(min);
    return true;
}

bool IMathFP::r(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t c = stack.pop();
    const int32_t n = stack.pop();

    stack.push(std::abs(c) >= 32 ? 0 : c >= 0 ? n >> c : n << -c);
    return true;
}

bool IMathFP::s(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    if(n < 0) {
        stack.push(n);
        return false;
    }

    int32_t sum = 0;
    for(int i = 0; i < n; i++) {
        sum += stack.pop();
    }

    stack.push(sum);
    return true;
}

bool IMathFP::t(InstructionPointer& ip) {
    Stack& stack = ip.getStack();

    // Get as unsigned to protect against overflow.
    const auto n = static_cast<uint32_t>(stack.pop());

    stack.push(static_cast<int32_t>(n * 10));
    return true;
}

bool IMathFP::u(InstructionPointer& ip) {
    const uint32_t n = static_cast<uint32_t>(ip.getStack().pop());
    std::cout << n << " ";
    return true;
}

bool IMathFP::x(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    if(n <= 0) {
        stack.push(n);
        return false;
    }

    int32_t max = std::numeric_limits<int32_t>::min();
    for(int i = 0; i < n; i++) {
        const int32_t val = stack.pop();
        max = std::min(max, val);
    }

    stack.push(max);
    return true;
}

bool IMathFP::z(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t val = stack.pop();

    // Guard against overflow if the min value happens to be passed
    stack.push(val == std::numeric_limits<int32_t>::min() ? val : -val);
    return true;
}