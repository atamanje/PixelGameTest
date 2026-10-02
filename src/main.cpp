#include "pch.h"
#include "Application.h"

int main(int argc, char** argv) {
    Application* app = new Application("PixelGameTest", 1280, 720);
    app->Run();
    delete app;
    return 0;
}
