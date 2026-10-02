#include "pch.h"
#include "Application.h"
#include "GameManager.h"

#include "../vendor/stb_image/stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

Application::Application(const char* title, int width, int height)
    : m_title(title), m_width(width), m_height(height) {}

Application::~Application() {}

void Application::Run() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(this->m_width, this->m_height, this->m_title, nullptr, nullptr);
    if (window == nullptr)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize Glad!" << std::endl;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    m_gameManager = std::make_unique<GameManager>();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        float deltaTime = ImGui::GetIO().DeltaTime;
        if (deltaTime > 0.1f) deltaTime = 0.1f; // Cap max frame time to avoid physics tunneling

        // Update Game Logic
        m_gameManager->Update(deltaTime);

        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool first_time = true;
        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        if (first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

            ImGuiID dock_id_left = 0;
            ImGuiID dock_id_right = 0;
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, &dock_id_left, &dock_id_right);

            ImGui::DockBuilderDockWindow("Controls", dock_id_left);
            ImGui::DockBuilderDockWindow("Viewport", dock_id_right);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        this->RenderGlobalMenuBar();

        // Render Inspector Controls
        m_gameManager->RenderControlPanel();

        // Render Game Scene in Viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        ImVec2 vSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);
        ImVec2 vPos = ImGui::GetWindowPos();
        ImVec2 vContentPos = ImVec2(vPos.x + vMin.x, vPos.y + vMin.y);

        m_gameManager->RenderViewport(vSize, vContentPos);

        ImGui::End();
        ImGui::PopStyleVar();

        // OpenGL Render Frame
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    m_gameManager.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::RenderGlobalMenuBar() {
    static bool show_about_popup = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::MenuItem("About PixelGameTest")) {
                show_about_popup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (show_about_popup) {
        ImGui::OpenPopup("About PixelGameTest");
        show_about_popup = false;
    }
    if (ImGui::BeginPopupModal("About PixelGameTest", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Welcome to PixelGameTest Engine");
        ImGui::SeparatorText("Version");
        ImGui::Text("Version 0.1.0 (2D Game Architecture)");
        ImGui::SeparatorText("Author");
        ImGui::Text("Created by: Jessie Atamanchuk");
        ImGui::SeparatorText("License Information");
        ImGui::Text("License: Do whatever you want, I don't care.");

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
