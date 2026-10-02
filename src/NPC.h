#pragma once
#include "Character.h"
#include "Texture2D.h"
#include "AnimatedSprite.h"
#include <functional>

class NPC : public Character {
public:
    std::string dialogueId;
    std::string defaultGreeting = "Hello traveler!";
    std::function<void(NPC*)> onInteractCallback;
    std::function<std::string(const NPC*)> dialogueResolver = nullptr;

    NPC(const std::string& name, Vec2 pos, uint32_t color, const std::string& dialogueId = "");
    virtual ~NPC() = default;

    std::string GetDialogueId() const;

    bool LoadSprite(const std::string& spritePath, int totalFrames = 4);

    virtual void Update(float deltaTime, World& world) override;
    virtual void OnCollision(Entity* instigator) override;
    virtual void Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) override;

private:
    std::shared_ptr<Texture2D> m_spriteTex;
    AnimatedSprite m_idleSprite;
};
