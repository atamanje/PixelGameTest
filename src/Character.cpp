#include "pch.h"
#include "Character.h"
#include "World.h"

Character::Character(const std::string& name, Vec2 pos, Vec2 size, uint32_t color)
    : Entity(name, pos, size, color) {}

Vec2 Character::GetFacingVector() const {
    switch (facing) {
        case Direction::Up:    return Vec2(0.0f, -1.0f);
        case Direction::Down:  return Vec2(0.0f, 1.0f);
        case Direction::Left:  return Vec2(-1.0f, 0.0f);
        case Direction::Right: return Vec2(1.0f, 0.0f);
    }
    return Vec2(0.0f, 1.0f);
}

void Character::Move(Vec2 inputDir, float deltaTime, World& world) {
    if (inputDir.LengthSquared() <= 0.001f) {
        isMoving = false;
        velocity = Vec2(0, 0);
        return;
    }

    isMoving = true;
    Vec2 dir = inputDir.Normalized();

    // Update facing direction based on dominant input axis
    if (std::abs(dir.x) > std::abs(dir.y)) {
        facing = (dir.x > 0) ? Direction::Right : Direction::Left;
    } else {
        facing = (dir.y > 0) ? Direction::Down : Direction::Up;
    }

    velocity = dir * moveSpeed;
    Vec2 deltaPos = velocity * deltaTime;

    // Axis-aligned collision move (X-axis then Y-axis for smooth sliding along walls)
    Vec2 newPosX = position + Vec2(deltaPos.x, 0.0f);
    if (!world.CheckCollision(newPosX, size, this)) {
        position.x = newPosX.x;
    }

    Vec2 newPosY = position + Vec2(0.0f, deltaPos.y);
    if (!world.CheckCollision(newPosY, size, this)) {
        position.y = newPosY.y;
    }
}
