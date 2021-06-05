workspace "EmNovel"
	configurations { "Debug", "Release" }
	architecture "x86_64"

project "EmNovel"
	kind "WindowedApp"
	
	language "C++"
	cppdialect "C++17"
	links { "SDL2.lib", "SDL2_image.lib", "SDL2_ttf.lib" }
	
	files { "*.cpp", "*.hpp" }
	
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "On"