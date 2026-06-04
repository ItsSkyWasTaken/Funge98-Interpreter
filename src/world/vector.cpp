#include "vector.hpp"

#include <cassert>
#include <random>

Vector Vector::origin(const int32_t dim) {
    switch(dim) {
        case 1:
            return {0};
        case 2:
            return {0, 0};
        default:
            assert(dim == 3);
            return {0, 0, 0};
    }
}

Vector Vector::east(const int32_t dim) {
    switch(dim) {
        case 1:
            return {1};
        case 2:
            return {1, 0};
        default:
            assert(dim == 3);
            return {1, 0, 0};
    }
}

Vector Vector::random(const int32_t dim) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution d(0, 2 * dim - 1);
    const int r = d(gen);
    int32_t coords[3] = {0, 0, 0};
    coords[r / 2] = r % 2 == 0 ? 1 : -1;

    switch(dim) {
        case 1:
            return {coords[0]};
        case 2:
            return {coords[0], coords[1]};
        default:
            assert(dim == 3);
            return {coords[0], coords[1], coords[2]};
    }
}

Vector::Vector(const int32_t x): x(x), y(0), z(0), dimensions(1) {}
Vector::Vector(const int32_t x, const int32_t y): x(x), y(y), z(0), dimensions(2) {}
Vector::Vector(const int32_t x, const int32_t y, const int32_t z): x(x), y(y), z(z), dimensions(3) {}

Vector Vector::operator+(const Vector& vector) const {
    switch(dimensions) {
        case 1:
            return {x + vector.x};
        case 2:
            return {x + vector.x, y + vector.y};
        default:
            return {x + vector.x, y + vector.y, z + vector.z};
    }
}

Vector Vector::operator-(const Vector& vector) const {
    switch(dimensions) {
        case 1:
            return {x - vector.x};
        case 2:
            return {x - vector.x, y - vector.y};
        default:
            return {x - vector.x, y - vector.y, z - vector.z};
    }
}

Vector Vector::operator*(const int scalar) const {
    switch(dimensions) {
        case 1:
            return {x * scalar};
        case 2:
            return {x * scalar, y * scalar};
        default:
            return {x * scalar, y * scalar, z * scalar};
    }
}

Vector operator*(const int scalar, const Vector& vector) {
    return vector * scalar;
}

Vector Vector::operator-() const {
    return *this * -1;
}

void Vector::operator+=(const Vector& vector) {
    x += vector.x;
    y += vector.y;
    z += vector.z;
}

void Vector::operator-=(const Vector& vector) {
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
}

void Vector::operator*=(const int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

Vector& Vector::operator=(const Vector& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

bool Vector::operator==(const Vector& v) const {
    return x == v.x && y == v.y && z == v.z && dimensions == v.dimensions;
}

int32_t Vector::getX() const {
    return x;
}

int32_t Vector::getY() const {
    return y;
}

int32_t Vector::getZ() const {
    return z;
}

std::size_t std::hash<Vector>::operator()(const Vector& v) const noexcept {
    const std::size_t h1 = std::hash<int>{}(v.x);
    const std::size_t h2 = std::hash<int>{}(v.y);
    const std::size_t h3 = std::hash<int>{}(v.z);
    const std::size_t h4 = std::hash<int>{}(v.dimensions);

    return h1 ^ h2 << 1 ^ h3 << 2 ^ h4 << 3;
}