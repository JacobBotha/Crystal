workspace "Crystal"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

    flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VulkanSDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "Crystal/vendor/GLFW/include"
IncludeDir["glm"] = "Crystal/vendor/glm"
IncludeDir["ImGui"] = "Crystal/vendor/ImGui"

include "Crystal/vendor/GLFW"
include "Crystal/vendor/ImGui"

project "Crystal"
	location "Crystal"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "clpch.h"
    pchsource "Crystal/src/clpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{VulkanSDK}/Include",
        "%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}"
	}

    links
    {
        "GLFW",
        "%{VulkanSDK}/Lib/vulkan-1.lib",
		"ImGui"
    }

	defines
	{
		"CL_PLATFORM_WINDOWS",
		"CL_BUILD_DLL",
		"GLFW_INCLUDE_VULKAN"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "On"
		buildoptions "/MDd"

	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"
		buildoptions "/MDd"

	filter "configurations:Dist"
		defines "CL_DIST"
		optimize "On"
		buildoptions "/MDd"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Crystal/vendor/spdlog/include",
		"Crystal/src",
        "%{IncludeDir.glm}"
	}

	links
	{
		"Crystal"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "On"
		buildoptions "/MDd"

	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"
		buildoptions "/MDd"

	filter "configurations:Dist"
		defines "CL_DIST"
		optimize "On" 
		buildoptions "/MDd"