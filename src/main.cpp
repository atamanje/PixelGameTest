// main.cpp
#include "super_luminal_pch.h"
#include "Application.h"

int main(int argc, char** argv) {
    // 1. Setup
    Application* app = new Application("Super Luminal Flight Traker", 1280, 720);

    // 2. Run
    app->Run();

    // 3. Cleanup
    delete app;
    return 0;
}
