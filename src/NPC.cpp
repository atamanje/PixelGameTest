#include "pch.h"
#include "NPC.h"

NPC::NPC(const std::string& name, Vec2 pos, uint32_t color, const std::string& dialogueId)
    : Character(name, pos, Vec2(32.0f, 32.0f), color), dialogueId(dialogueId) {
    type = EntityType::NPC;
    m_spriteTex = std::make_shared<Texture2D>();
}

std::string NPC::GetDialogueId() const {
    if (dialogueResolver) {
        return dialogueResolver(this);
    }
    return dialogueId;
}

bool NPC::LoadSprite(const std::string& spritePath, int totalFrames) {
    if (m_spriteTex->LoadFromFile(spritePath)) {
        m_idleSprite.SetTexture(m_spriteTex, 32, 32, totalFrames);
        m_idleSprite.SetFPS(6.0f);
        return true;
    }
    return false;
}

void NPC::Update(float deltaTime, World& world) {
    if (m_spriteTex && m_spriteTex->IsLoaded()) {
        m_idleSprite.Update(deltaTime);
    }
}

void NPC::OnCollision(Entity* instigator) {
    if (instigator) {
        Vec2 diff = instigator->position - position;
        if (std::abs(diff.x) > std::abs(diff.y)) {
            facing = (diff.x > 0.0f) ? Direction::Right : Direction::Left;
        } else {
            facing = (diff.y > 0.0f) ? Direction::Down : Direction::Up;
        }
    }
    if (onInteractCallback) {
        onInteractCallback(this);
    }
}

void NPC::Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) {
    if (!isActive || !drawList) return;

    Vec2 screenMin = camera.WorldToScreen(position, viewportSize, viewportOrigin);
    Vec2 screenMax = camera.WorldToScreen(position + size, viewportSize, viewportOrigin);

    if (m_idleSprite.GetTexture() && m_idleSprite.GetTexture()->IsLoaded()) {
        // Shadow underneath NPC feet
        Vec2 shadowCenter = camera.WorldToScreen(position + Vec2(size.x * 0.5f, size.y - 2.0f), viewportSize, viewportOrigin);
        float shadowRadiusX = (size.x * 0.4f) * camera.zoom;
        float shadowRadiusY = (size.y * 0.15f) * camera.zoom;
        drawList->AddEllipseFilled(ImVec2(shadowCenter.x, shadowCenter.y), ImVec2(shadowRadiusX, shadowRadiusY), IM_COL32(0, 0, 0, 90));

        float u0, v0, u1, v1;
        m_idleSprite.GetUVs(m_idleSprite.GetCurrentFrame(), u0, v0, u1, v1);

        drawList->AddImage(
            (ImTextureID)(intptr_t)m_idleSprite.GetTexture()->GetID(),
            ImVec2(screenMin.x, screenMin.y),
            ImVec2(screenMax.x, screenMax.y),
            ImVec2(u0, v0),
            ImVec2(u1, v1)
        );
    } else {
        Entity::Render(drawList, camera, viewportSize, viewportOrigin);
    }

    // Draw NPC Name tag above character
    Vec2 nameScreen = camera.WorldToScreen(position + Vec2(-8.0f, -18.0f), viewportSize, viewportOrigin);
    drawList->AddText(ImVec2(nameScreen.x, nameScreen.y), IM_COL32(255, 255, 200, 255), name.c_str());
}
