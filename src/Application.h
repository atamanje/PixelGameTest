#pragma once

class Application
{
public:
	Application(const char* title, int height, int width);
	~Application();
	void Run();

private:
	const char* m_title;
	int m_height;
	int m_width;

	void RenderGlobalMenuBar();
};