#include "Stack.hpp"

void Stack::initialize() {
    globalStack.emplace_back();
}

void Stack::push(const int32_t n) {
    toss() -> stack.push(n);
}

void Stack::push(const char c) {
    toss() -> stack.push(c);
}

void Stack::push(const Vector& v) {
    std::stack<int32_t>& stack = toss() -> stack;

    switch(v.dimensions) {
        case 1:
            stack.push(v.getX());
            break;
        case 2:
            stack.push(v.getX());
            stack.push(v.getY());
            break;
        default:
            stack.push(v.getX());
            stack.push(v.getY());
            stack.push(v.getZ());
    }
}

void Stack::push(const std::string& s) {
    std::stack<int32_t>& stack = toss() -> stack;

    stack.push(0);

    for(size_t i = s.size(); i > 0; i--) {
        stack.push(s[i - 1]);
    }
}

int32_t Stack::pop() {
    std::stack<int32_t>& stack = toss() -> stack;
    if(stack.empty()) {
        return 0;
    }

    const int32_t result = stack.top();
    stack.pop();
    return result;
}

char Stack::popChar() {
    std::stack<int32_t>& stack = toss() -> stack;
    if(stack.empty()) {
        return '\0';
    }

    const char result = static_cast<char>(stack.top());
    stack.pop();
    return result;
}

Vector Stack::popVector(const int dimensions) {
    int32_t x, y, z;

    switch(dimensions) {
        case 1:
            x = pop();
            return {x};
        case 2:
            y = pop();
            x = pop();
            return {x, y};
        default:
            z = pop();
            y = pop();
            x = pop();
            return {x, y, z};
    }
}

std::string Stack::popString() {
    std::string result;
    result.reserve(toss() -> stack.size());

    char c = popChar();
    while(c != '\0') {
        result += c;
        c = popChar();
    }

    return result;
}

Stack* Stack::toss() {
    return &globalStack[globalStack.size() - 1];
}

Stack* Stack::soss() {
    return globalStack.size() < 2 ? nullptr : &globalStack[globalStack.size() - 2];
}