#include "pch.h"
#include <gtest/gtest.h>
#include "../src/Vec2.h"
#include "../src/Camera2D.h"
#include "../src/Entity.h"
#include "../src/DialogueSystem.h"

TEST(Vec2Test, MathOperations) {
    Vec2 a(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.Length(), 5.0f);

    Vec2 b(1.0f, 2.0f);
    Vec2 c = a + b;
    EXPECT_FLOAT_EQ(c.x, 4.0f);
    EXPECT_FLOAT_EQ(c.y, 6.0f);

    Vec2 norm = a.Normalized();
    EXPECT_NEAR(norm.Length(), 1.0f, 0.0001f);
}

TEST(Camera2DTest, WorldScreenTransform) {
    Camera2D cam(Vec2(100.0f, 100.0f), 2.0f);
    Vec2 viewportSize(800.0f, 600.0f);
    Vec2 viewportOrigin(0.0f, 0.0f);

    // Camera center should map to viewport center (400, 300)
    Vec2 screenCenter = cam.WorldToScreen(cam.position, viewportSize, viewportOrigin);
    EXPECT_FLOAT_EQ(screenCenter.x, 400.0f);
    EXPECT_FLOAT_EQ(screenCenter.y, 300.0f);

    // Test inverse transform
    Vec2 worldPos(150.0f, 200.0f);
    Vec2 screen = cam.WorldToScreen(worldPos, viewportSize, viewportOrigin);
    Vec2 restoredWorld = cam.ScreenToWorld(screen, viewportSize, viewportOrigin);
    EXPECT_NEAR(restoredWorld.x, worldPos.x, 0.001f);
    EXPECT_NEAR(restoredWorld.y, worldPos.y, 0.001f);
}

TEST(DialogueSystemTest, NodeChoiceBranching) {
    DialogueSystem sys;

    DialogueNode start;
    start.id = "start";
    start.speaker = "NPC";
    start.text = "Hello!";
    start.choices.push_back({ "Go to node 2", "node2", nullptr });

    DialogueNode node2;
    node2.id = "node2";
    node2.speaker = "NPC";
    node2.text = "Welcome to node 2!";

    sys.RegisterNode(start);
    sys.RegisterNode(node2);

    sys.StartDialogue("start");
    EXPECT_TRUE(sys.isActive);
    EXPECT_EQ(sys.currentNode.id, "start");

    sys.SelectChoice(0);
    EXPECT_TRUE(sys.isActive);
    EXPECT_EQ(sys.currentNode.id, "node2");
}

TEST(DialogueSystemTest, StoryFlagsAndVariables) {
    DialogueSystem sys;
    EXPECT_FALSE(sys.GetFlag("talked_to_elder"));
    EXPECT_EQ(sys.GetVariable("quest_step"), 0);

    sys.SetFlag("talked_to_elder", true);
    sys.SetVariable("quest_step", 2);

    EXPECT_TRUE(sys.GetFlag("talked_to_elder"));
    EXPECT_EQ(sys.GetVariable("quest_step"), 2);

    sys.ClearStoryState();
    EXPECT_FALSE(sys.GetFlag("talked_to_elder"));
    EXPECT_EQ(sys.GetVariable("quest_step"), 0);
}

TEST(DialogueSystemTest, ConditionalChoices) {
    DialogueSystem sys;

    DialogueNode node;
    node.id = "shop";
    node.speaker = "Shopkeeper";
    node.text = "What can I do for you?";

    // Choice 0: Always available
    node.choices.push_back({ "Browse goods", "", nullptr });
    // Choice 1: Available only if has_coupon flag is true
    node.choices.push_back({ "Use discount coupon", "", nullptr, [&sys]() {
        return sys.GetFlag("has_coupon");
    }});

    sys.RegisterNode(node);
    sys.StartDialogue("shop");

    // Initially has_coupon is false
    auto avail1 = sys.GetAvailableChoiceIndices();
    EXPECT_EQ(avail1.size(), 1);
    EXPECT_EQ(avail1[0], 0);

    // Set flag and restart dialogue
    sys.SetFlag("has_coupon", true);
    sys.StartDialogue("shop");
    auto avail2 = sys.GetAvailableChoiceIndices();
    EXPECT_EQ(avail2.size(), 2);
}

TEST(DialogueSystemTest, TypewriterEffect) {
    DialogueSystem sys;
    sys.typewriterEnabled = true;
    sys.typewriterSpeed = 10.0f; // 10 chars per second

    DialogueNode node;
    node.id = "intro";
    node.speaker = "Guide";
    node.text = "Welcome to the world!";
    sys.RegisterNode(node);

    sys.StartDialogue("intro");
    EXPECT_TRUE(sys.isActive);
    EXPECT_FALSE(sys.isTextComplete);
    EXPECT_EQ(sys.GetVisibleText(), "");

    // Advance 0.5s -> 5 chars revealed
    sys.Update(0.5f);
    EXPECT_EQ(sys.GetVisibleText(), "Welco");
    EXPECT_FALSE(sys.isTextComplete);

    // Instant reveal
    sys.RevealFullText();
    EXPECT_TRUE(sys.isTextComplete);
    EXPECT_EQ(sys.GetVisibleText(), "Welcome to the world!");
}

TEST(DialogueSystemTest, ChoiceNavigationAndConfirm) {
    DialogueSystem sys;
    sys.typewriterEnabled = false;

    bool choice1Selected = false;
    bool choice2Selected = false;

    DialogueNode node;
    node.id = "menu";
    node.speaker = "Host";
    node.text = "Pick an option:";
    node.choices.push_back({ "Option 1", "", [&]() { choice1Selected = true; } });
    node.choices.push_back({ "Option 2", "", [&]() { choice2Selected = true; } });
    sys.RegisterNode(node);

    sys.StartDialogue("menu");
    EXPECT_EQ(sys.selectedChoiceIndex, 0);

    sys.NextChoice();
    EXPECT_EQ(sys.selectedChoiceIndex, 1);

    sys.PrevChoice();
    EXPECT_EQ(sys.selectedChoiceIndex, 0);

    sys.NextChoice();
    sys.ConfirmSelection(); // Confirms choice index 1
    EXPECT_TRUE(choice2Selected);
    EXPECT_FALSE(choice1Selected);
    EXPECT_FALSE(sys.isActive); // Dialog closed because nextNodeId is empty
}


#include "../src/AnimatedSprite.h"

TEST(AnimatedSpriteTest, FrameSteppingAndUVs) {
    AnimatedSprite sprite;
    sprite.SetTexture(nullptr, 32, 32, 4);
    sprite.SetFPS(10.0f); // 0.1s per frame

    EXPECT_EQ(sprite.GetCurrentFrame(), 0);
    EXPECT_TRUE(sprite.IsPlaying());

    // Advance 0.05s -> still frame 0
    sprite.Update(0.05f);
    EXPECT_EQ(sprite.GetCurrentFrame(), 0);

    // Advance another 0.06s -> advances to frame 1
    sprite.Update(0.06f);
    EXPECT_EQ(sprite.GetCurrentFrame(), 1);

    // Reset -> back to frame 0
    sprite.Reset();
    EXPECT_EQ(sprite.GetCurrentFrame(), 0);
}

#include "../src/Texture2D.h"

TEST(TextureLoadingTest, MultiPathResolution) {
    Texture2D tex;
    bool loaded = tex.LoadFromFile("resources/sprites/hero_idle.png");
    EXPECT_TRUE(loaded);
    EXPECT_TRUE(tex.IsLoaded());
    EXPECT_GT(tex.GetWidth(), 0);
    EXPECT_GT(tex.GetHeight(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
