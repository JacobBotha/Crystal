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
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"GLFW_INCLUDE_VULKAN"
	}

	filter "system:windows"
		systemversion "latest"

		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		-- }

	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "On"
		-- buildoptions "/MDd"

		links 
		{
        	"%{VulkanSDK}/Lib/shaderc_combinedd.lib",
        	-- "%{VulkanSDK}/Lib/spirv-cross-cored.lib",
        	-- "%{VulkanSDK}/Lib/spirv-cross-glsld.lib",
        	-- "%{VulkanSDK}/Lib/SPIRV-Toolsd.lib",
		}

	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "On"
		-- buildoptions "/MDd"

		links 
		{
        	"%{VulkanSDK}/Lib/shaderc_combined.lib",
		}

	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "On"
		-- buildoptions "/MDd"

		links 
		{
        	"%{VulkanSDK}/Lib/shaderc_combined.lib",
		}

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
        "%{IncludeDir.glm}",
        "%{VulkanSDK}/Include",
	}

	links
	{
		"Crystal"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "on"
		runtime "Debug"
		-- buildoptions "/MDd"

	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "on"
		runtime "Release"
		-- buildoptions "/MDd"

	filter "configurations:Dist"
		defines "CL_DIST"
		optimize "on" 
		runtime "Release"
		-- buildoptions "/MDd"