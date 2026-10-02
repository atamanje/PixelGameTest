workspace "SuperLuminalFlightTraker"
    architecture "x64"
    startproject "Super Luminal Flight Traker"

    configurations { "Debug", "Release" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Define paths
    IncludeDir = {}
    IncludeDir["GLFW"] = "vendor/glfw/include"
    IncludeDir["Glad"] = "vendor/glad/include"
    IncludeDir["ImGui"] = "vendor/imgui"
    IncludeDir["ImPlot"] = "vendor/implot"
    IncludeDir["Curl"] = "vendor/curl/include"
    IncludeDir["JSON"] = "vendor/json/include"
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

-- Project: Curl (build from source)
project "Curl"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "vendor/curl/include",
        "vendor/curl/lib"
    }

    files {
        "vendor/curl/lib/**.c",
        "vendor/curl/lib/**.h",
    }
	
	removefiles {
        "vendor/curl/lib/ldap.c",
        "vendor/curl/lib/ldaps.c",
        "vendor/curl/lib/openldap.c",
        "vendor/curl/lib/vtls/openssl.c",
        "vendor/curl/lib/vtls/mbedtls.c",
        "vendor/curl/lib/vtls/wolfssl.c",
    }

    defines {
        "BUILDING_LIBCURL",
        "CURL_STATICLIB",
        "USE_SCHANNEL",
        "USE_WINDOWS_SSPI",
        "USE_WIN32_IDN",
		"CURL_DISABLE_LDAP",
        "CURL_DISABLE_LDAPS",
        "WANT_IDN_PROTOTYPES",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

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
        "vendor/imgui/imgui_demo.cpp", --optional
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

-- Project: implot (build from source)
project "implot"
    kind "StaticLib"
    language "C++"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
    files {
        "vendor/implot/implot.cpp",
        "vendor/implot/implot_items.cpp"
    }
    includedirs {
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.ImPlot}",
    }
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
project "Super Luminal Flight Traker"
    location "."
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	--Precompiled header
	pchheader "super_luminal_pch.h"
	pchsource "src/super_luminal_pch.cpp"	

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
        "%{IncludeDir.ImGui}/backends",
		"%{IncludeDir.ImPlot}",
		"%{IncludeDir.Curl}",
		"%{IncludeDir.JSON}"
    }

    links {
        "GLFW",
        "opengl32",
		"gdi32",
        "user32",
        "kernel32",
        "shell32",
		"Curl",
        "ws2_32",
        "crypt32",
        "wldap32",
		"secur32",
        "normaliz",
        "advapi32",
		"glad",
		"imgui",
		"implot",
    }

    filter "system:windows"
        systemversion "latest"
        entrypoint "mainCRTStartup"
        
        defines { 
            "_GLFW_WIN32", 
            "_CRT_SECURE_NO_WARNINGS",
            "CURL_STATICLIB"
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
	pchheader "super_luminal_pch.h"
	pchsource "src/super_luminal_pch.cpp"

    files {
        "./tests/**.cpp",
        "./src/PhysicsCalculations.cpp",
        "./src/super_luminal_pch.cpp",
    }

    includedirs {
        "./src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGui}/backends",
		"%{IncludeDir.ImPlot}",
        "%{IncludeDir.GTest}"
    }

    links {
        "gtest"
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
