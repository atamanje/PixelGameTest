#include "super_luminal_pch.h"
#include "Application.h"
#include "SimulationData.h"
#include "InputLayer.h"
#include "CalculationLayer.h"
#include "VisualizationLayer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image/stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    //fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Application::Application(const char* title, int height, int width)
: m_title(title), m_height(height), m_width(width) {};

Application::~Application() {};

void Application::Run() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(this->m_width, this->m_height, "Super Luminal Flight Traker", nullptr, nullptr);
    if (window == nullptr)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        //std::cout << "Failed to initialize Glad!" << std::endl;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SimulationData simData;
    simData.star_db.loadFromGaiaAPIAsync(1000);

    InputLayer inputLayer(&simData);
    CalculationLayer calculationLayer(&simData);
    VisualizationLayer visualizationLayer(&simData);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- APP CODE GOES HERE ---
        static bool first_time = true;
        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        if (first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

            ImGuiID dock_id_left = 0;
            ImGuiID dock_id_right = 0;
            ImGuiID dock_id_NW = 0;
            ImGuiID dock_id_SW = 0;
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, &dock_id_left, &dock_id_right);
            ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.50f, &dock_id_SW, &dock_id_NW);

            // Assign windows to dock positions
            ImGui::DockBuilderDockWindow("Data Window", dock_id_NW);
            ImGui::DockBuilderDockWindow("Calculation Window", dock_id_SW);
            ImGui::DockBuilderDockWindow("Plotting Window", dock_id_right);

            ImGui::DockBuilderFinish(dockspace_id);
        }
        this->RenderGlobalMenuBar();

        inputLayer.render();
        calculationLayer.render();
        visualizationLayer.render();

		//ImPlot::ShowDemoWindow();
        // -------------------------------

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return;
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
        if (ImGui::BeginMenu("Edit"))
        {
            // TODO
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            // TODO
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            // TODO
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
			
            if (ImGui::MenuItem("About Super Luminal Flight Traker")) {
				show_about_popup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (show_about_popup) {
        ImGui::OpenPopup("About Super Luminal Flight Traker");
		show_about_popup = false;
    }
    if (ImGui::BeginPopupModal("About Super Luminal Flight Traker", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static GLuint app_icon_texture = 0;
        static int app_icon_width = 0;
        static int app_icon_height = 0;
        if (app_icon_texture == 0) {
            unsigned char* image_data = stbi_load("resources/app.png", &app_icon_width, &app_icon_height, NULL, 4);
            if (image_data != NULL) {
                glGenTextures(1, &app_icon_texture);
                glBindTexture(GL_TEXTURE_2D, app_icon_texture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app_icon_width, app_icon_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
                stbi_image_free(image_data);
            } else {
                app_icon_texture = (GLuint)-1;
            }
        }
        
        if (app_icon_texture != 0 && app_icon_texture != (GLuint)-1) {
            float img_width = 128.0f;
            float img_height = (float)app_icon_height / (float)app_icon_width * img_width;
            float window_width = ImGui::GetWindowSize().x;
            float center_pos = (window_width - img_width) * 0.5f;
            if (center_pos > 0.0f) ImGui::SetCursorPosX(center_pos);
            ImGui::Image((ImTextureID)(intptr_t)app_icon_texture, ImVec2(img_width, img_height));
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::Text("Welcome to Super Luminal Flight Traker");
        ImGui::SeparatorText("Version");
        ImGui::Text("Version 0.0.0 (WIP)");
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
