#pragma once
#include "World.h"
#include "Camera2D.h"
#include "Player.h"
#include "NPC.h"
#include "DialogueSystem.h"

enum class GameState {
    Exploration,
    Dialogue,
    Pause
};

class GameManager {
public:
    World world;
    Camera2D camera;
    Player* player = nullptr;
    DialogueSystem dialogueSystem;
    GameState state = GameState::Exploration;

    GameManager();

    void InitializeSampleScene();

    void Update(float deltaTime);
    void RenderViewport(ImVec2 viewportSize, ImVec2 viewportPos);
    void RenderControlPanel();

private:
    void RenderDialogueUI();
};
