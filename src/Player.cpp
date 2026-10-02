#include "pch.h"
#include "Player.h"
#include "World.h"

Player::Player(Vec2 startPos)
    : Character("Hero", startPos, Vec2(32.0f, 32.0f), IM_COL32(50, 150, 250, 255)) {
    type = EntityType::Player;
    moveSpeed = 160.0f;

    m_idleTex = std::make_shared<Texture2D>();
    m_walkTex = std::make_shared<Texture2D>();
}

bool Player::LoadSprites(const std::string& idlePath, const std::string& walkPath) {
    bool ok1 = m_idleTex->LoadFromFile(idlePath);
    if (ok1) {
        m_idleSprite.SetTexture(m_idleTex, 32, 32, 4);
        m_idleSprite.SetFPS(8.0f);
    }

    bool ok2 = m_walkTex->LoadFromFile(walkPath);
    if (ok2) {
        m_walkSprite.SetTexture(m_walkTex, 32, 32, 4);
        m_walkSprite.SetFPS(8.0f);
    }

    return ok1 && ok2;
}

void Player::Update(float deltaTime, World& world) {
    Vec2 inputDir(0.0f, 0.0f);

    // Read ImGui Keyboard state for WASD / Arrow keys
    if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow)) inputDir.y -= 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow)) inputDir.y += 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow)) inputDir.x -= 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow)) inputDir.x += 1.0f;

    if (inputDir.x < 0.0f) m_flipX = true;
    else if (inputDir.x > 0.0f) m_flipX = false;

    Move(inputDir, deltaTime, world);

    // Advance active animation sprite
    AnimatedSprite& activeSprite = isMoving ? m_walkSprite : m_idleSprite;
    activeSprite.Update(deltaTime);

    // Trigger Interaction on Space or Enter key press
    if (ImGui::IsKeyPressed(ImGuiKey_Space) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        TryInteract(world);
    }
}

void Player::TryInteract(World& world) {
    Vec2 interactBoxPos = position + GetFacingVector() * 24.0f;
    Vec2 interactBoxSize = size;

    Entity* target = world.FindEntityInRect(interactBoxPos, interactBoxSize, this);
    if (target) {
        target->OnCollision(this);
    }
}

void Player::Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) {
    if (!isActive || !drawList) return;

    Vec2 screenMin = camera.WorldToScreen(position, viewportSize, viewportOrigin);
    Vec2 screenMax = camera.WorldToScreen(position + size, viewportSize, viewportOrigin);

    AnimatedSprite& activeSprite = isMoving ? m_walkSprite : m_idleSprite;

    if (activeSprite.GetTexture() && activeSprite.GetTexture()->IsLoaded()) {
        // Render soft oval shadow at feet
        Vec2 shadowCenter = camera.WorldToScreen(position + Vec2(size.x * 0.5f, size.y - 2.0f), viewportSize, viewportOrigin);
        float shadowRadiusX = (size.x * 0.4f) * camera.zoom;
        float shadowRadiusY = (size.y * 0.15f) * camera.zoom;
        drawList->AddEllipseFilled(ImVec2(shadowCenter.x, shadowCenter.y), ImVec2(shadowRadiusX, shadowRadiusY), IM_COL32(0, 0, 0, 90));

        // Get frame UVs
        float u0, v0, u1, v1;
        activeSprite.GetUVs(activeSprite.GetCurrentFrame(), u0, v0, u1, v1);

        // Flip horizontally if facing left
        if (m_flipX) {
            std::swap(u0, u1);
        }

        drawList->AddImage(
            (ImTextureID)(intptr_t)activeSprite.GetTexture()->GetID(),
            ImVec2(screenMin.x, screenMin.y),
            ImVec2(screenMax.x, screenMax.y),
            ImVec2(u0, v0),
            ImVec2(u1, v1)
        );
    } else {
        // Fallback to bounding box rendering
        Entity::Render(drawList, camera, viewportSize, viewportOrigin);
    }

    // Facing indicator line
    Vec2 screenCenter = camera.WorldToScreen(position + size * 0.5f, viewportSize, viewportOrigin);
    Vec2 facingScreen = camera.WorldToScreen(position + size * 0.5f + GetFacingVector() * 16.0f, viewportSize, viewportOrigin);
    drawList->AddLine(ImVec2(screenCenter.x, screenCenter.y), ImVec2(facingScreen.x, facingScreen.y), IM_COL32(255, 255, 255, 180), 1.5f);
}
