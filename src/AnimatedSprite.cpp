#include "pch.h"
#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite()
    : m_texture(nullptr), m_frameWidth(0), m_frameHeight(0), m_totalFrames(0),
      m_currentFrame(0), m_fps(8.0f), m_timer(0.0f), m_isPlaying(true) {}

AnimatedSprite::~AnimatedSprite() {}

void AnimatedSprite::SetTexture(std::shared_ptr<Texture2D> texture, int frameWidth, int frameHeight, int totalFrames) {
    m_texture = texture;
    m_frameWidth = frameWidth;
    m_frameHeight = frameHeight;
    m_totalFrames = totalFrames;
    m_currentFrame = 0;
    m_timer = 0.0f;
}

void AnimatedSprite::Update(float deltaTime) {
    if (!m_isPlaying || m_totalFrames <= 1 || m_fps <= 0.0f)
        return;

    m_timer += deltaTime;
    float frameTime = 1.0f / m_fps;

    if (m_timer >= frameTime) {
        m_timer -= frameTime;
        m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
    }
}

void AnimatedSprite::SetCurrentFrame(int frame) {
    if (frame >= 0 && frame < m_totalFrames) {
        m_currentFrame = frame;
    }
}

void AnimatedSprite::GetUVs(int frameIndex, float& u0, float& v0, float& u1, float& v1) const {
    if (!m_texture || !m_texture->IsLoaded() || m_texture->GetWidth() == 0) {
        u0 = 0.0f; v0 = 0.0f; u1 = 1.0f; v1 = 1.0f;
        return;
    }

    int texW = m_texture->GetWidth();
    int texH = m_texture->GetHeight();

    int framesPerRow = texW / m_frameWidth;
    if (framesPerRow <= 0) framesPerRow = 1;

    int col = frameIndex % framesPerRow;
    int row = frameIndex / framesPerRow;

    u0 = (float)(col * m_frameWidth) / (float)texW;
    v0 = (float)(row * m_frameHeight) / (float)texH;
    u1 = (float)((col + 1) * m_frameWidth) / (float)texW;
    v1 = (float)((row + 1) * m_frameHeight) / (float)texH;
}
