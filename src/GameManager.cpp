#include "pch.h"
#include "GameManager.h"

GameManager::GameManager() {
    InitializeSampleScene();
}

void GameManager::InitializeSampleScene() {
    // Load Pixel Art World Tileset
    world.LoadTileset("resources/tilesets/world_tiles.png");
    world.GenerateDefaultMap();

    // Spawn Player at path start and load pixel art character animations
    player = world.AddEntity<Player>(Vec2(12.0f * 32.0f + 2.0f, 15.0f * 32.0f));
    player->LoadSprites("resources/sprites/hero_idle.png", "resources/sprites/hero_walk.png");

    camera.position = player->position + player->size * 0.5f;
    camera.zoom = 4.0f; // Detailed 4.0x pixel zoom for clear character view

    // ----------------------------------------------------
    // Story Nodes: Elder Elion
    // ----------------------------------------------------
    DialogueNode greeting;
    greeting.id = "elder_greet";
    greeting.speaker = "Elder Elion";
    greeting.text = "Greetings, brave traveler! Welcome to Pixel Vale. Whispers tell of strange disturbances in the northern woods.";
    greeting.choices.push_back({ "What can I do to help?", "elder_quest_start", nullptr });
    greeting.choices.push_back({ "I am just exploring for now.", "elder_explore", nullptr });
    greeting.choices.push_back({ "Farewell, Elder.", "", nullptr });

    DialogueNode questStart;
    questStart.id = "elder_quest_start";
    questStart.speaker = "Elder Elion";
    questStart.text = "Courageous spirit! Before venturing north, seek out Merchant Bob by the eastern pond. He can prepare you with vital supplies.";
    questStart.choices.push_back({ "I will speak with Bob immediately.", "", [this]() {
        dialogueSystem.SetFlag("met_elder", true);
    }});

    DialogueNode exploreChoice;
    exploreChoice.id = "elder_explore";
    exploreChoice.speaker = "Elder Elion";
    exploreChoice.text = "Take your time to know our vale. When you are ready, speak with Merchant Bob by the pond.";
    exploreChoice.choices.push_back({ "Understood, Elder.", "", [this]() {
        dialogueSystem.SetFlag("met_elder", true);
    }});

    DialogueNode remindBob;
    remindBob.id = "elder_remind_bob";
    remindBob.speaker = "Elder Elion";
    remindBob.text = "Have you visited Merchant Bob by the eastern pond yet? Make sure you have supplies before going further.";
    remindBob.choices.push_back({ "I'm heading over to him now.", "", nullptr });

    DialogueNode questReward;
    questReward.id = "elder_quest_reward";
    questReward.speaker = "Elder Elion";
    questReward.text = "I see Bob has given you an Elixir of Vigor! You now possess the strength to watch over Pixel Vale. Keep this village safe!";
    questReward.choices.push_back({ "Pixel Vale can count on me!", "", [this]() {
        dialogueSystem.SetFlag("quest_complete", true);
    }});

    DialogueNode elderFinal;
    elderFinal.id = "elder_final";
    elderFinal.speaker = "Elder Elion";
    elderFinal.text = "May the stars guide your path, protector of the Vale.";
    elderFinal.choices.push_back({ "Thank you, Elder.", "", nullptr });

    dialogueSystem.RegisterNode(greeting);
    dialogueSystem.RegisterNode(questStart);
    dialogueSystem.RegisterNode(exploreChoice);
    dialogueSystem.RegisterNode(remindBob);
    dialogueSystem.RegisterNode(questReward);
    dialogueSystem.RegisterNode(elderFinal);

    // ----------------------------------------------------
    // Story Nodes: Merchant Bob
    // ----------------------------------------------------
    DialogueNode bobInitial;
    bobInitial.id = "bob_initial";
    bobInitial.speaker = "Merchant Bob";
    bobInitial.text = "Welcome to Bob's Pondside Goods! You seem new here. Have you paid your respects to Elder Elion up by the path yet?";
    bobInitial.choices.push_back({ "I will go speak with him first.", "", nullptr });

    DialogueNode bobGivePotion;
    bobGivePotion.id = "bob_give_potion";
    bobGivePotion.speaker = "Merchant Bob";
    bobGivePotion.text = "Ah! Elder Elion mentioned a capable traveler was in town. Take this Elixir of Vigor--free of charge for our future hero!";
    bobGivePotion.choices.push_back({ "Thank you, Bob! I'll report back to the Elder.", "", [this]() {
        dialogueSystem.SetFlag("received_bob_potion", true);
    }});

    DialogueNode bobAfterPotion;
    bobAfterPotion.id = "bob_after_potion";
    bobAfterPotion.speaker = "Merchant Bob";
    bobAfterPotion.text = "Keep that Elixir handy! Let me know if you need anything else once my full inventory arrives.";
    bobAfterPotion.choices.push_back({ "Good luck with business, Bob.", "", nullptr });

    dialogueSystem.RegisterNode(bobInitial);
    dialogueSystem.RegisterNode(bobGivePotion);
    dialogueSystem.RegisterNode(bobAfterPotion);

    // Spawn NPC Elder near the path and load Elder pixel art sprite
    NPC* elder = world.AddEntity<NPC>("Elder Elion", Vec2(12.0f * 32.0f + 2.0f, 8.0f * 32.0f), IM_COL32(230, 180, 50, 255), "elder_greet");
    elder->LoadSprite("resources/sprites/elder_idle.png", 4);
    elder->dialogueResolver = [this](const NPC*) -> std::string {
        if (!dialogueSystem.GetFlag("met_elder")) return "elder_greet";
        if (!dialogueSystem.GetFlag("received_bob_potion")) return "elder_remind_bob";
        if (!dialogueSystem.GetFlag("quest_complete")) return "elder_quest_reward";
        return "elder_final";
    };
    elder->onInteractCallback = [this](NPC* npc) {
        state = GameState::Dialogue;
        dialogueSystem.StartDialogue(npc->GetDialogueId());
    };

    // Spawn NPC Merchant near the pond
    NPC* merchant = world.AddEntity<NPC>("Merchant Bob", Vec2(18.0f * 32.0f, 5.0f * 32.0f), IM_COL32(200, 100, 200, 255), "bob_initial");
    merchant->dialogueResolver = [this](const NPC*) -> std::string {
        if (!dialogueSystem.GetFlag("met_elder")) return "bob_initial";
        if (!dialogueSystem.GetFlag("received_bob_potion")) return "bob_give_potion";
        return "bob_after_potion";
    };
    merchant->onInteractCallback = [this](NPC* npc) {
        state = GameState::Dialogue;
        dialogueSystem.StartDialogue(npc->GetDialogueId());
    };
}

void GameManager::Update(float deltaTime) {
    if (state == GameState::Exploration) {
        world.Update(deltaTime);
        if (player) {
            camera.Follow(player->position + player->size * 0.5f, 6.0f, deltaTime);
        }
    } else if (state == GameState::Dialogue) {
        dialogueSystem.Update(deltaTime);
        if (!dialogueSystem.isActive) {
            state = GameState::Exploration;
        }
    }
}

void GameManager::RenderViewport(ImVec2 viewportSize, ImVec2 viewportPos) {
    // Interactive mouse wheel zoom in viewport
    if (ImGui::IsWindowHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            camera.zoom = std::clamp(camera.zoom + wheel * 0.25f, 1.0f, 8.0f);
        }
    }

    Vec2 vSize(viewportSize.x, viewportSize.y);
    Vec2 vOrigin(viewportPos.x, viewportPos.y);

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Clip draw calls strictly inside Viewport window
    drawList->PushClipRect(viewportPos, ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y), true);

    // Render World (Tiles & Entities)
    world.Render(drawList, camera, vSize, vOrigin);

    // Render Dialogue Overlay if active
    if (dialogueSystem.isActive) {
        RenderDialogueUI();
    }

    drawList->PopClipRect();
}

void GameManager::RenderDialogueUI() {
    if (!dialogueSystem.isActive) return;

    // Handle Keyboard Navigation
    auto availableChoices = dialogueSystem.GetAvailableChoiceIndices();

    // Number keys (1-9)
    for (size_t i = 0; i < availableChoices.size() && i < 9; ++i) {
        if (ImGui::IsKeyPressed((ImGuiKey)(ImGuiKey_1 + i))) {
            dialogueSystem.SelectChoice(availableChoices[i]);
            break;
        }
    }

    // Up / Down arrow or W / S
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) || ImGui::IsKeyPressed(ImGuiKey_W)) {
        dialogueSystem.PrevChoice();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) || ImGui::IsKeyPressed(ImGuiKey_S)) {
        dialogueSystem.NextChoice();
    }

    // Enter / Space to advance or confirm
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_Space)) {
        dialogueSystem.ConfirmSelection();
    }

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + (vp->Size.x - 520.0f) * 0.5f, vp->Pos.y + vp->Size.y - 210.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(520, 180), ImGuiCond_Always);

    ImGui::Begin("Dialogue", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "[ %s ]", dialogueSystem.currentNode.speaker.c_str());
    ImGui::Separator();

    std::string visibleText = dialogueSystem.GetVisibleText();
    ImGui::TextWrapped("%s", visibleText.c_str());
    ImGui::Spacing();
    ImGui::Separator();

    if (availableChoices.empty()) {
        ImGui::TextDisabled("[ Space / Enter ] Close dialogue");
    } else {
        for (int i = 0; i < (int)availableChoices.size(); ++i) {
            int choiceIdx = availableChoices[i];
            const auto& choice = dialogueSystem.currentNode.choices[choiceIdx];
            bool isSelected = (dialogueSystem.selectedChoiceIndex == choiceIdx);

            std::string prefix = isSelected ? "> " : "  ";
            std::string label = prefix + std::to_string(i + 1) + ". " + choice.text;

            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.70f));
            }
            if (ImGui::Button(label.c_str())) {
                dialogueSystem.SelectChoice(choiceIdx);
            }
            if (isSelected) {
                ImGui::PopStyleColor();
            }
        }
    }

    ImGui::End();
}

void GameManager::RenderControlPanel() {
    ImGui::Begin("Controls");
    ImGui::Text("Game State: %s", (state == GameState::Exploration ? "Exploration" : (state == GameState::Dialogue ? "Dialogue" : "Pause")));
    ImGui::Separator();

    if (player) {
        ImGui::Text("Player Pos: (%.1f, %.1f)", player->position.x, player->position.y);
        ImGui::Text("Moving: %s", player->isMoving ? "Yes" : "No");
        ImGui::SliderFloat("Move Speed", &player->moveSpeed, 50.0f, 400.0f, "%.0f px/s");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Story & Quest State:");
    ImGui::BulletText("Met Elder: %s", dialogueSystem.GetFlag("met_elder") ? "Yes" : "No");
    ImGui::BulletText("Received Bob's Elixir: %s", dialogueSystem.GetFlag("received_bob_potion") ? "Yes" : "No");
    ImGui::BulletText("Northern Quest Complete: %s", dialogueSystem.GetFlag("quest_complete") ? "Yes" : "No");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Camera Controls");
    ImGui::Text("Cam Pos: (%.1f, %.1f)", camera.position.x, camera.position.y);
    ImGui::SliderFloat("Zoom", &camera.zoom, 1.0f, 8.0f, "%.2fx");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Pixel Art Assets Loaded:");
    ImGui::BulletText("Tileset: world_tiles.png (32x32)");
    ImGui::BulletText("Hero: hero_idle.png & hero_walk.png");
    ImGui::BulletText("NPC: elder_idle.png");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Instructions:");
    ImGui::BulletText("WASD / Arrows: Move Hero");
    ImGui::BulletText("Space / Enter: Talk to NPCs");
    ImGui::BulletText("Dialogue: 1-9 or Up/Down + Enter to choose");
    ImGui::BulletText("Mouse Wheel in Viewport: Zoom In/Out");

    ImGui::End();
}
