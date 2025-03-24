workspace "Fluid-Material-Editor"
    architecture "x64"
   
    configurations{
        "Debug",
        "Release",
        "Dist"
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "vendors/glfw/include"
IncludeDir["glad"] = "vendors/glad/include"
IncludeDir["ImGui"] = "vendors/imgui/include"
IncludeDir["glm"] = "vendors/glm"
IncludeDir["entt"] = "vendors/entt"

include "vendors/glfw"
include "vendors/glad"
include "vendors/imgui"

project "Fluid-Material-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
   
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "fmepch.h"
    pchsource "src/fmepch.cpp"

    files{
        "src/**.h",
        "src/**.cpp",
        "vendors/glm/**.hpp",
        "vendors/glm/**.inl",
    }

    defines{
        "_CRT_SECURE_NO_WARNINGS"
    }

    links{
        "glfw",
        "glad",
        "imgui",
        "opengl32.lib"
    }

    includedirs
    {
        "src",
        "vendors/spdlog/include",
        "%{IncludeDir.entt}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }

    buildoptions {"/utf-8"}

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "FME_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
        defines {"PE_DEBUG","PE_ENABLE_ASSERTS"}
        symbols "on"            

    filter "configurations:Release"
        defines "PE_RELEASE"
        symbols "on"

    filter "configurations:Dist"
        defines "PE_DIST"
        optimize "on"