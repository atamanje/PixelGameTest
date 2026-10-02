#pragma once
#include "Vec2.h"
#include "Camera2D.h"
#include <string>

enum class EntityType {
    Generic,
    Player,
    NPC,
    Object
};

class World; // Forward declaration

class Entity {
public:
    int id = 0;
    std::string name = "Entity";
    EntityType type = EntityType::Generic;

    Vec2 position{ 0.0f, 0.0f };
    Vec2 size{ 32.0f, 32.0f };
    Vec2 velocity{ 0.0f, 0.0f };
    
    bool isSolid = true;
    bool isActive = true;
    uint32_t color = 0xFFFFFFFF; // ABGR color format for rendering

    Entity() = default;
    Entity(const std::string& name, Vec2 pos, Vec2 size, uint32_t color);
    virtual ~Entity() = default;

    virtual void Update(float deltaTime, World& world) = 0;
    virtual void Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin);

    // Bounding Box Collision test (AABB)
    bool CollidesWith(const Entity& other) const;
    bool CollidesWithRect(Vec2 rectPos, Vec2 rectSize) const;

    virtual void OnCollision(Entity* other) {}
};
