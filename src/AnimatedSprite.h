#pragma once
#include "pch.h"
#include "Texture2D.h"

class AnimatedSprite {
public:
    AnimatedSprite();
    ~AnimatedSprite();

    void SetTexture(std::shared_ptr<Texture2D> texture, int frameWidth, int frameHeight, int totalFrames);
    void Update(float deltaTime);

    void Play() { m_isPlaying = true; }
    void Pause() { m_isPlaying = false; }
    void Reset() { m_currentFrame = 0; m_timer = 0.0f; }
    
    void SetFPS(float fps) { if (fps > 0.0f) m_fps = fps; }
    float GetFPS() const { return m_fps; }

    void SetCurrentFrame(int frame);
    int GetCurrentFrame() const { return m_currentFrame; }
    int GetTotalFrames() const { return m_totalFrames; }
    bool IsPlaying() const { return m_isPlaying; }

    void GetUVs(int frameIndex, float& u0, float& v0, float& u1, float& v1) const;

    std::shared_ptr<Texture2D> GetTexture() const { return m_texture; }
    int GetFrameWidth() const { return m_frameWidth; }
    int GetFrameHeight() const { return m_frameHeight; }

private:
    std::shared_ptr<Texture2D> m_texture;
    int m_frameWidth;
    int m_frameHeight;
    int m_totalFrames;
    int m_currentFrame;
    
    float m_fps;
    float m_timer;
    bool m_isPlaying;
};
