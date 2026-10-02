#pragma once
#include "Vec2.h"

class Camera2D {
public:
    Vec2 position{ 0.0f, 0.0f }; // World position of camera center
    float zoom = 4.0f;           // Zoom factor (4.0 = close pixel detail)

    Camera2D() = default;
    Camera2D(Vec2 startPos, float startZoom = 4.0f);

    // Transforms world position into viewport screen pixel coordinates
    Vec2 WorldToScreen(const Vec2& worldPos, const Vec2& viewportSize, const Vec2& viewportOrigin) const;

    // Transforms viewport screen coordinates back into world position
    Vec2 ScreenToWorld(const Vec2& screenPos, const Vec2& viewportSize, const Vec2& viewportOrigin) const;

    // Smoothly tracks target position (e.g. Player)
    void Follow(const Vec2& targetPos, float lerpSpeed, float deltaTime);
};
