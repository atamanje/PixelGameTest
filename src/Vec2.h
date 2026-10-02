#pragma once
#include <cmath>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
    Vec2 operator-(const Vec2& rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

    Vec2& operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vec2& operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }

    bool operator==(const Vec2& rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const Vec2& rhs) const { return !(*this == rhs); }

    float Length() const { return std::sqrt(x * x + y * y); }
    float LengthSquared() const { return x * x + y * y; }

    Vec2 Normalized() const {
        float len = Length();
        if (len > 0.00001f) {
            return Vec2(x / len, y / len);
        }
        return Vec2(0.0f, 0.0f);
    }

    static float Distance(const Vec2& a, const Vec2& b) {
        return (a - b).Length();
    }
};
