#pragma once
#include "pch.h"

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    bool LoadFromFile(const std::string& filePath);
    void Unload();

    unsigned int GetID() const { return m_textureID; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool IsLoaded() const { return m_isLoaded || m_textureID != 0; }

private:
    unsigned int m_textureID;
    int m_width;
    int m_height;
    int m_channels;
    bool m_isLoaded = false;
};
