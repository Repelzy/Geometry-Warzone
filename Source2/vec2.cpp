#include "vec2.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
    :x(xin), y(yin) {}

bool Vec2::operator == (const Vec2& rhs) const {
    return (x==rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const {
    return (x != rhs.x && y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const {
    return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const {
    return Vec2(x/val, y/val);
}

void Vec2::operator +=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -=(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator *=(const float val) {
    x *= val;
    y *= val;
}

void Vec2::operator /=(const float val) {
    x /= val;
    y /= val;
}

float Vec2::dist(const Vec2& rhs) const {
    return sqrt(x * rhs.x + y * rhs.y);
}

Vec2 Vec2::normalize() const {
    double L = sqrt(x * x + y * y);
    if (L != 0) {  // Avoid division by zero
        return Vec2(x / L, y / L);
    }
    else {
        // Handle zero-length vector case
        return Vec2(0, 0);
    }
}

float Vec2::length() const
{
    return sqrt(x*x + y * y);
}

