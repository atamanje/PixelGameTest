#pragma once
#include "Entity.h"

enum class Direction {
    Down,
    Up,
    Left,
    Right
};

class Character : public Entity {
public:
    float moveSpeed = 120.0f; // Pixels / units per second
    Direction facing = Direction::Down;
    bool isMoving = false;

    Character(const std::string& name, Vec2 pos, Vec2 size, uint32_t color);
    virtual ~Character() = default;

    virtual void Move(Vec2 direction, float deltaTime, World& world);
    virtual void Interact(Entity* instigator, World& world) {}

    Vec2 GetFacingVector() const;
};
