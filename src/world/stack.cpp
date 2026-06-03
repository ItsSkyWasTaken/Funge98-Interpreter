#include "stack.hpp"
#include "vector.hpp"

Stack::Stack() {
    ss.emplace_back();
}

void Stack::push(const int32_t n) {
    toss() -> push(n);
}

void Stack::push(const char32_t c) {
    toss() -> push(static_cast<int32_t>(c));
}

void Stack::push(const std::float32_t f) {
    toss() -> push(std::bit_cast<int32_t>(f));
}

void Stack::push(const std::float64_t d) {
    std::stack<int32_t>& stack = *toss();

    const auto bits = std::bit_cast<uint64_t>(d);
    const auto high = static_cast<int32_t>(bits >> 32);
    const auto low = static_cast<int32_t>(bits & 0xFFFFFFFF);

    stack.push(high);
    stack.push(low);
}

void Stack::push(const Vector& v) {
    std::stack<int32_t>& stack = *toss();

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

void Stack::push(const std::u32string& s) {
    std::stack<int32_t>& stack = *toss();

    stack.push(0);

    for(size_t i = s.size(); i > 0; i--) {
        push(s[i - 1]);
    }
}

int32_t Stack::peek() {
    return toss()->empty() ? 0 : toss()->top();
}

void Stack::duplicate() {
    if(std::stack<int32_t>& stack = *toss(); stack.empty()) {
        stack.push(0);
    } else {
        stack.push(stack.top());
    }
}

void Stack::clear() {
    std::stack<int32_t>& stack = *toss();

    while(!stack.empty()) {
        stack.pop();
    }
}

int32_t Stack::pop() {
    std::stack<int32_t>& stack = *toss();
    if(stack.empty()) {
        return 0;
    }

    const int32_t result = stack.top();
    stack.pop();
    return result;
}

char32_t Stack::popChar() {
    std::stack<int32_t>& stack = *toss();
    if(stack.empty()) {
        return U'\0';
    }

    const auto result = static_cast<char32_t>(stack.top());
    stack.pop();
    return result;
}

std::float32_t Stack::popFloat() {
    return std::bit_cast<std::float32_t>(pop());
}

std::float64_t Stack::popDouble() {
    const int32_t low = pop();
    const int32_t high = pop();

    const uint64_t combined = static_cast<uint64_t>(static_cast<uint32_t>(high)) << 32 | static_cast<uint32_t>(low);

    return std::bit_cast<std::float64_t>(combined);
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

std::u32string Stack::popString() {
    std::u32string result;
    result.reserve(toss() -> size());

    char32_t c = popChar();
    while(c != U'\0') {
        result += c;
        c = popChar();
    }

    return result;
}

uint32_t Stack::size() const {
    return ss.size();
}

void Stack::split(const int32_t elements) {
    ss.emplace_back();

    std::stack<int32_t> temp;
    if(elements <= 0) {
        for(int i = 0; i > elements; i--) {
            soss()->push(0);
        }
    } else if(elements > soss()->size()) {
        for(int i = 0; i < elements - soss()->size(); i++) {
            toss()->push(0);
        }

        while(!soss()->empty()) {
            temp.push(soss()->top());
            soss()->pop();
        }

        while(!temp.empty()) {
            toss()->push(temp.top());
            temp.pop();
        }
    } else {
        for(int i = 0; i < elements; i++) {
            temp.push(soss()->top());
            soss()->pop();
        }

        while(!temp.empty()) {
            toss()->push(temp.top());
            temp.pop();
        }
    }
}

void Stack::collapse(const int32_t elements) {
    if(soss() == nullptr) {
        return;
    }

    std::stack<int32_t> temp;
    if(elements <= 0) {
        for(int i = 0; i > elements; i--) {
            soss()->pop();
        }
    } else if(elements > toss()->size()) {
        for(int i = 0; i < elements - toss()->size(); i++) {
            soss()->push(0);
        }

        while(!toss()->empty()) {
            temp.push(toss()->top());
            toss()->pop();
        }

        while(!temp.empty()) {
            soss()->push(temp.top());
            temp.pop();
        }
    } else {
        for(int i = 0; i < elements; i++) {
            temp.push(toss()->top());
            toss()->pop();
        }

        while(!temp.empty()) {
            soss()->push(temp.top());
            temp.pop();
        }
    }

    ss.pop_back();
}

void Stack::transfer(const int32_t elements) {
    if(soss() == nullptr) {
        return;
    }

    std::stack<int32_t> temp;
    if(elements >= 0) {
        for(int i = 0; i < elements; i++) {
            if(soss()->empty()) {
                toss()->push(0);
            } else {
                toss()->push(soss()->top());
                soss()->pop();
            }
        }
    } else {
        for(int i = 0; i > elements; i--) {
            if(toss()->empty()) {
                soss()->push(0);
            } else {
                soss()->push(toss()->top());
                toss()->pop();
            }
        }
    }
}

std::stack<int32_t>* Stack::toss() {
    return &ss[ss.size() - 1];
}

std::stack<int32_t>* Stack::soss() {
    return ss.size() < 2 ? nullptr : &ss[ss.size() - 2];
}