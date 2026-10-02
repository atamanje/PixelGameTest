#pragma once
#include "Character.h"
#include "Texture2D.h"
#include "AnimatedSprite.h"

class Player : public Character {
public:
    Player(Vec2 startPos = Vec2(100.0f, 100.0f));
    virtual ~Player() = default;

    bool LoadSprites(const std::string& idlePath, const std::string& walkPath);

    virtual void Update(float deltaTime, World& world) override;
    virtual void Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) override;

    void TryInteract(World& world);

private:
    std::shared_ptr<Texture2D> m_idleTex;
    std::shared_ptr<Texture2D> m_walkTex;
    AnimatedSprite m_idleSprite;
    AnimatedSprite m_walkSprite;
    bool m_flipX = false;
};
