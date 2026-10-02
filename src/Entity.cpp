#include "pch.h"
#include "Entity.h"

Entity::Entity(const std::string& name, Vec2 pos, Vec2 size, uint32_t color)
    : name(name), position(pos), size(size), color(color) {}

void Entity::Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) {
    if (!isActive || !drawList) return;

    // Convert top-left world position to screen coordinates
    Vec2 screenMin = camera.WorldToScreen(position, viewportSize, viewportOrigin);
    Vec2 screenMax = camera.WorldToScreen(position + size, viewportSize, viewportOrigin);

    // Draw Entity bounding box / sprite quad
    drawList->AddRectFilled(ImVec2(screenMin.x, screenMin.y), ImVec2(screenMax.x, screenMax.y), color, 4.0f);
    drawList->AddRect(ImVec2(screenMin.x, screenMin.y), ImVec2(screenMax.x, screenMax.y), IM_COL32(0, 0, 0, 255), 4.0f, 0, 1.5f);
}

bool Entity::CollidesWith(const Entity& other) const {
    if (!isActive || !other.isActive) return false;
    return CollidesWithRect(other.position, other.size);
}

bool Entity::CollidesWithRect(Vec2 rectPos, Vec2 rectSize) const {
    return (position.x < rectPos.x + rectSize.x &&
            position.x + size.x > rectPos.x &&
            position.y < rectPos.y + rectSize.y &&
            position.y + size.y > rectPos.y);
}
