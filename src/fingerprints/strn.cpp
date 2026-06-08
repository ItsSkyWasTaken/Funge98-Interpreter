#include "strn.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "../world/pointer.hpp"
#include "../world/stack.hpp"
#include "../world/world.hpp"
#include "../strings.hpp"
#include "fingerprint.hpp"

std::shared_ptr<StringFP> StringFP::instance() {
    if(!inst) {
        inst = std::make_shared<StringFP>(FingerprintToken{});
    }

    return inst;
}

StringFP::StringFP(const FingerprintToken token): Fingerprint(token, false) {
    implementedInstructions.insert(implementedInstructions.end(), {U'A', U'C', U'D', U'F', U'G', U'I', U'L', U'M', U'N', U'P', U'R', U'S', U'V'});
}

bool StringFP::a(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    std::u32string b = stack.popString();
    const std::u32string a = stack.popString();

    b.append(a);
    stack.push(b);
    return true;
}

bool StringFP::c(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const std::u32string a = stack.popString(), b = stack.popString();

    stack.push(a.compare(b));
    return true;
}

bool StringFP::d(InstructionPointer& ip) {
    std::cout << Strings::toUtf8(ip.getStack().popString());
    return true;
}

bool StringFP::f(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const std::u32string haystack = stack.popString(), needle = stack.popString();
    const size_t position = haystack.find(needle);

    stack.push(position == std::string::npos ? U"" : haystack.substr(position));
    return true;
}

bool StringFP::g(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    Vector v = stack.popVector(world->dimensions) + ip.getOffset();
    const Vector east = Vector::east(world->dimensions);
    std::u32string result;

    char32_t c = world->get(v);
    while(c != U'\0') {
        result += c;
        v += east;
        if(!world->boundsCheck(v, east)) {
            return false;
        }

        c = world->get(v);
    }

    stack.push(result);
    return true;
}

bool StringFP::i(InstructionPointer& ip) {
    std::string input;
    std::cin >> input;
    ip.getStack().push(Strings::fromUtf8(input));
    return true;
}

bool StringFP::l(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();
    const std::u32string s = stack.popString();

    if(n < 0) {
        return false;
    }

    stack.push(s.substr(0, std::min(static_cast<size_t>(n), s.length())));
    return true;
}

bool StringFP::m(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t length = stack.pop();
    const int32_t start = stack.pop();
    const std::u32string string = stack.popString();

    if(start < 0 || length < 0 || start > string.length()) {
        return false;
    }

    stack.push(string.substr(start, std::min(static_cast<size_t>(length), string.length() - start)));
    return true;
}

bool StringFP::n(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const std::u32string s = stack.popString();

    stack.push(s);
    stack.push(static_cast<int>(s.size()));
    return true;
}

bool StringFP::p(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    Vector v = stack.popVector(world->dimensions) + ip.getOffset();
    const std::u32string s = stack.popString();
    const Vector east = Vector::east(world->dimensions);

    // Guard against overflow past world limit
    if(v.getX() + static_cast<int64_t>(s.length()) + 1 > std::numeric_limits<std::int32_t>::max()) {
        return false;
    }

    for(const char32_t c : s) {
        world->put(v, c);
        v += east;
    }

    world->put(v, U'\0');
    return true;
}

bool StringFP::r(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();
    const std::u32string s = stack.popString();

    if(n < 0) {
        return false;
    }

    stack.push(s.substr(static_cast<size_t>(std::max(0, static_cast<int32_t>(s.length()) - n))));
    return true;
}

bool StringFP::s(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const int32_t n = stack.pop();

    stack.push(Strings::fromUtf8(std::to_string(n)));
    return true;
}

bool StringFP::v(InstructionPointer& ip) {
    Stack& stack = ip.getStack();
    const std::string s = Strings::toUtf8(stack.popString());

    int32_t r;

    try {
        r = std::stoi(s);
    } catch(const std::logic_error& _) {
        r = 0;
    }

    stack.push(r);
    return true;
}