#include "pch.h"
#include "DialogueSystem.h"

DialogueSystem::DialogueSystem() {
}

void DialogueSystem::SetFlag(const std::string& key, bool value) {
    m_flags[key] = value;
}

bool DialogueSystem::GetFlag(const std::string& key, bool defaultValue) const {
    auto it = m_flags.find(key);
    if (it != m_flags.end()) {
        return it->second;
    }
    return defaultValue;
}

void DialogueSystem::SetVariable(const std::string& key, int value) {
    m_variables[key] = value;
}

int DialogueSystem::GetVariable(const std::string& key, int defaultValue) const {
    auto it = m_variables.find(key);
    if (it != m_variables.end()) {
        return it->second;
    }
    return defaultValue;
}

void DialogueSystem::ClearStoryState() {
    m_flags.clear();
    m_variables.clear();
}

void DialogueSystem::RegisterNode(const DialogueNode& node) {
    m_nodes[node.id] = node;
}

const DialogueNode* DialogueSystem::GetNode(const std::string& nodeId) const {
    auto it = m_nodes.find(nodeId);
    if (it != m_nodes.end()) {
        return &it->second;
    }
    return nullptr;
}

void DialogueSystem::StartDialogue(const std::string& startNodeId) {
    const DialogueNode* node = GetNode(startNodeId);
    if (node) {
        currentNode = *node;
        isActive = true;
        
        // Pick first available choice index
        auto available = GetAvailableChoiceIndices();
        selectedChoiceIndex = available.empty() ? 0 : available[0];

        visibleChars = typewriterEnabled ? 0.0f : (float)currentNode.text.size();
        isTextComplete = !typewriterEnabled || currentNode.text.empty();

        if (currentNode.onEnter) {
            currentNode.onEnter();
        }
    } else {
        CloseDialogue();
    }
}

void DialogueSystem::Update(float deltaTime) {
    if (!isActive) return;

    if (!isTextComplete && typewriterEnabled) {
        visibleChars += typewriterSpeed * deltaTime;
        if (visibleChars >= (float)currentNode.text.size()) {
            visibleChars = (float)currentNode.text.size();
            isTextComplete = true;
        }
    }
}

void DialogueSystem::RevealFullText() {
    visibleChars = (float)currentNode.text.size();
    isTextComplete = true;
}

std::string DialogueSystem::GetVisibleText() const {
    if (!typewriterEnabled || isTextComplete) {
        return currentNode.text;
    }
    size_t count = static_cast<size_t>(visibleChars);
    if (count > currentNode.text.size()) {
        count = currentNode.text.size();
    }
    return currentNode.text.substr(0, count);
}

std::vector<int> DialogueSystem::GetAvailableChoiceIndices() const {
    std::vector<int> available;
    for (int i = 0; i < (int)currentNode.choices.size(); ++i) {
        if (currentNode.choices[i].IsAvailable()) {
            available.push_back(i);
        }
    }
    return available;
}

void DialogueSystem::NextChoice() {
    auto available = GetAvailableChoiceIndices();
    if (available.empty()) return;

    int currPos = 0;
    for (size_t i = 0; i < available.size(); ++i) {
        if (available[i] == selectedChoiceIndex) {
            currPos = (int)i;
            break;
        }
    }
    currPos = (currPos + 1) % (int)available.size();
    selectedChoiceIndex = available[currPos];
}

void DialogueSystem::PrevChoice() {
    auto available = GetAvailableChoiceIndices();
    if (available.empty()) return;

    int currPos = 0;
    for (size_t i = 0; i < available.size(); ++i) {
        if (available[i] == selectedChoiceIndex) {
            currPos = (int)i;
            break;
        }
    }
    currPos = (currPos - 1 + (int)available.size()) % (int)available.size();
    selectedChoiceIndex = available[currPos];
}

void DialogueSystem::ConfirmSelection() {
    if (!isActive) return;

    if (!isTextComplete) {
        RevealFullText();
        return;
    }

    auto available = GetAvailableChoiceIndices();
    if (available.empty()) {
        CloseDialogue();
        return;
    }

    SelectChoice(selectedChoiceIndex);
}

void DialogueSystem::SelectChoice(int choiceIndex) {
    if (!isActive || choiceIndex < 0 || choiceIndex >= (int)currentNode.choices.size()) {
        CloseDialogue();
        return;
    }

    const auto& choice = currentNode.choices[choiceIndex];
    if (choice.onSelect) {
        choice.onSelect();
    }

    if (choice.nextNodeId.empty()) {
        CloseDialogue();
    } else {
        StartDialogue(choice.nextNodeId);
    }
}

void DialogueSystem::CloseDialogue() {
    isActive = false;
    selectedChoiceIndex = 0;
    visibleChars = 0.0f;
    isTextComplete = true;
}

