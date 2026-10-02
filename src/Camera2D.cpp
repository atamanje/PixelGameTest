#include "pch.h"
#include "Camera2D.h"
#include <algorithm>

Camera2D::Camera2D(Vec2 startPos, float startZoom)
    : position(startPos), zoom(startZoom) {}

Vec2 Camera2D::WorldToScreen(const Vec2& worldPos, const Vec2& viewportSize, const Vec2& viewportOrigin) const {
    Vec2 centerOffset = viewportSize * 0.5f;
    Vec2 relativePos = (worldPos - position) * zoom;
    return viewportOrigin + centerOffset + relativePos;
}

Vec2 Camera2D::ScreenToWorld(const Vec2& screenPos, const Vec2& viewportSize, const Vec2& viewportOrigin) const {
    Vec2 centerOffset = viewportSize * 0.5f;
    Vec2 relativeScreen = screenPos - viewportOrigin - centerOffset;
    if (zoom <= 0.0001f) return position;
    return position + (relativeScreen / zoom);
}

void Camera2D::Follow(const Vec2& targetPos, float lerpSpeed, float deltaTime) {
    float factor = std::clamp(lerpSpeed * deltaTime, 0.0f, 1.0f);
    position.x += (targetPos.x - position.x) * factor;
    position.y += (targetPos.y - position.y) * factor;
}
