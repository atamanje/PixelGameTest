#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

struct DialogueChoice {
    std::string text;
    std::string nextNodeId; // Empty string ends dialogue
    std::function<void()> onSelect;
    std::function<bool()> condition = nullptr; // Optional condition for choice availability

    bool IsAvailable() const {
        return !condition || condition();
    }
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogueChoice> choices;
    std::function<void()> onEnter = nullptr; // Optional callback when this node is displayed
};

class DialogueSystem {
public:
    bool isActive = false;
    DialogueNode currentNode;
    int selectedChoiceIndex = 0;

    // Typewriter effect state
    bool typewriterEnabled = true;
    float typewriterSpeed = 45.0f; // characters per second
    float visibleChars = 0.0f;
    bool isTextComplete = false;

    DialogueSystem();

    // Story State & Quest Flags
    void SetFlag(const std::string& key, bool value = true);
    bool GetFlag(const std::string& key, bool defaultValue = false) const;
    void SetVariable(const std::string& key, int value);
    int GetVariable(const std::string& key, int defaultValue = 0) const;
    void ClearStoryState();

    void RegisterNode(const DialogueNode& node);
    void StartDialogue(const std::string& startNodeId);
    void SelectChoice(int choiceIndex);
    void CloseDialogue();

    void Update(float deltaTime);
    void RevealFullText();
    std::string GetVisibleText() const;

    // Choice navigation
    std::vector<int> GetAvailableChoiceIndices() const;
    void NextChoice();
    void PrevChoice();
    void ConfirmSelection();

    const DialogueNode* GetNode(const std::string& nodeId) const;

private:
    std::map<std::string, DialogueNode> m_nodes;
    std::unordered_map<std::string, bool> m_flags;
    std::unordered_map<std::string, int> m_variables;
};

