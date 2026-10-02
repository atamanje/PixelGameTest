#pragma once
#include <memory>

class GameManager;

class Application
{
public:
    Application(const char* title, int width, int height);
    ~Application();
    void Run();

private:
    const char* m_title;
    int m_width;
    int m_height;

    std::unique_ptr<GameManager> m_gameManager;

    void RenderGlobalMenuBar();
};