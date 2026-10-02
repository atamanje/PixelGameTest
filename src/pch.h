#pragma once

// Standard headers
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <mutex>
#include <thread>

// Windows / OpenGL / Glad headers
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ImGui headers
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
