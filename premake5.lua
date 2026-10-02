workspace "PixelGameTest"
    architecture "x64"
    startproject "PixelGameTest"
    multiprocessorcompile "On"
    editandcontinue "Off"
    buildoptions { "/FS" }

    configurations { "Debug", "Release" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Define paths
    IncludeDir = {}
    IncludeDir["GLFW"] = "vendor/glfw/include"
    IncludeDir["Glad"] = "vendor/glad/include"
    IncludeDir["ImGui"] = "vendor/imgui"
    IncludeDir["GTest"] = "vendor/googletest/googletest/include"

-- Project: GLFW (Build from Source)
project "GLFW"
    location "vendor/glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "vendor/glfw/include/GLFW/glfw3.h",
        "vendor/glfw/include/GLFW/glfw3native.h",
        "vendor/glfw/src/context.c",
        "vendor/glfw/src/init.c",
        "vendor/glfw/src/input.c",
        "vendor/glfw/src/win32_module.c",
        "vendor/glfw/src/monitor.c",
        "vendor/glfw/src/platform.c",
        "vendor/glfw/src/vulkan.c",
        "vendor/glfw/src/window.c"
    }

    -- Platform-specific source files for GLFW
    filter "system:windows"
        systemversion "latest"
        
        files {
            "vendor/glfw/src/win32_init.c",
            "vendor/glfw/src/win32_joystick.c",
            "vendor/glfw/src/win32_monitor.c",
            "vendor/glfw/src/win32_time.c",
            "vendor/glfw/src/win32_thread.c",
            "vendor/glfw/src/win32_window.c",
            "vendor/glfw/src/wgl_context.c",
            "vendor/glfw/src/egl_context.c",
            "vendor/glfw/src/osmesa_context.c",
            
            "vendor/glfw/src/null_init.c",
            "vendor/glfw/src/null_joystick.c",
            "vendor/glfw/src/null_monitor.c",
            "vendor/glfw/src/null_window.c"
        }
        defines {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "ENABLE_VC_PROJECT_CACHE_LOGGING"
        }
        
    filter "system:linux"
        pic "On"
        systemversion "latest"
        files {
            "vendor/glfw/src/x11_init.c",
            "vendor/glfw/src/x11_monitor.c",
            "vendor/glfw/src/x11_window.c",
            "vendor/glfw/src/xkb_unicode.c",
            "vendor/glfw/src/linux_joystick.c",
            "vendor/glfw/src/posix_time.c",
            "vendor/glfw/src/posix_thread.c",
            "vendor/glfw/src/glx_context.c",
            "vendor/glfw/src/egl_context.c",
            "vendor/glfw/src/osmesa_context.c"
        }
        defines { "_GLFW_X11" }

    -- Configurations for GLFW
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

-- Project: glad (build from source)
project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files { "vendor/glad/src/glad.c" }
    includedirs { "vendor/glad/include" }
    filter "system:windows"
        systemversion "latest"
        
-- Project: imgui (build from source)
project "imgui"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files {
        "vendor/imgui/imgui.cpp",
        "vendor/imgui/imgui_demo.cpp",
        "vendor/imgui/imgui_draw.cpp",
        "vendor/imgui/imgui_tables.cpp",
        "vendor/imgui/imgui_widgets.cpp",
        "vendor/imgui/backends/imgui_impl_glfw.cpp",
        "vendor/imgui/backends/imgui_impl_opengl3.cpp"
    }
    includedirs {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
    }
    
    links { "GLFW" }
    
    filter "system:windows"
        systemversion "latest"

-- Project: googletest
project "gtest"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "vendor/googletest/googletest/src/gtest-all.cc"
    }

    includedirs {
        "vendor/googletest/googletest",
        "vendor/googletest/googletest/include"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

-- Project: Main Application
project "PixelGameTest"
    location "."
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    --Precompiled header
    pchheader "pch.h"
    pchsource "src/pch.cpp" 

    files {
        "./src/**.h",
        "./src/**.cpp",
        "./resources/**.rc",
    }

    includedirs {
        "./src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/backends"
    }

    links {
        "GLFW",
        "opengl32",
        "gdi32",
        "user32",
        "kernel32",
        "shell32",
        "advapi32",
        "glad",
        "imgui"
    }

    postbuildcommands {
        "{COPYDIR} %{wks.location}/resources %{cfg.targetdir}/resources"
    }

    filter "system:windows"
        systemversion "latest"
        entrypoint "mainCRTStartup"
        
        defines { 
            "_GLFW_WIN32", 
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "On"

-- Project: Unit Tests
project "Tests"
    location "."
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    --Precompiled header
    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files {
        "./tests/**.cpp",
        "./src/Camera2D.cpp",
        "./src/DialogueSystem.cpp",
        "./src/Entity.cpp",
        "./src/Character.cpp",
        "./src/Player.cpp",
        "./src/NPC.cpp",
        "./src/World.cpp",
        "./src/AnimatedSprite.cpp",
        "./src/Texture2D.cpp",
        "./src/pch.cpp"
    }

    includedirs {
        "./src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/backends",
        "%{IncludeDir.GTest}"
    }

    links {
        "gtest",
        "imgui",
        "GLFW",
        "glad",
        "opengl32"
    }

    filter "system:windows"
        systemversion "latest"
        
        defines { 
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "On"
