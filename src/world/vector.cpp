#include "vector.hpp"

Vector Vector::origin(const int8_t dim) {
    switch(dim) {
        case 1:
            return {0};
        case 2:
            return {0, 0};
        default:
            return {0, 0, 0};
    }
}

Vector Vector::east(const int8_t dim) {
    switch(dim) {
        case 1:
            return {1};
        case 2:
            return {1, 0};
        default:
            return {1, 0, 0};
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

int32_t Vector::getX() const {
    return x;
}

int32_t Vector::getY() const {
    return y;
}

int32_t Vector::getZ() const {
    return z;
}