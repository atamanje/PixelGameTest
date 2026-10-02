#include "pch.h"
#include "Texture2D.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "../vendor/stb_image/stb_image.h"

#include <filesystem>

Texture2D::Texture2D()
    : m_textureID(0), m_width(0), m_height(0), m_channels(0) {}

Texture2D::~Texture2D() {
    Unload();
}

bool Texture2D::LoadFromFile(const std::string& filePath) {
    Unload();

    std::vector<std::string> searchPaths = {
        filePath,
        "../../" + filePath,
        "../" + filePath,
        "../../../" + filePath,
        "./" + filePath
    };

    std::string validPath = "";
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            validPath = path;
            break;
        }
    }

    if (validPath.empty()) {
        validPath = filePath;
    }

    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(validPath.c_str(), &m_width, &m_height, &m_channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture from path: " << filePath << " (Resolved path: " << validPath << ")" << std::endl;
        return false;
    }

    if (glGenTextures == nullptr) {
        // Headless environment without active OpenGL context (e.g. unit tests)
        stbi_image_free(data);
        m_isLoaded = true;
        return true;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set texture filtering parameters: GL_NEAREST for sharp pixel art scaling!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    m_isLoaded = true;
    return true;
}

void Texture2D::Unload() {
    m_isLoaded = false;
    if (m_textureID != 0) {
        if (glDeleteTextures != nullptr) {
            glDeleteTextures(1, &m_textureID);
        }
        m_textureID = 0;
        m_width = 0;
        m_height = 0;
        m_channels = 0;
    }
}
