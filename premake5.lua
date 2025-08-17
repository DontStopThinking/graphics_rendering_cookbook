workspace "GraphicsCookbook"
    -- project files output location
    location "projects/%{_ACTION}/"

    -- Solution file name
    filename "%{wks.name:lower()}"

    -- Unity build
    enableunitybuild "On"

    -- Global defines
    defines { "_CRT_SECURE_NO_WARNINGS" }

    -- Directories
    objdir "tmp/%{cfg.buildcfg}/%{prj.name:lower()}"

    -- Configurations (i.e debug, release, etc.)
    configurations { "debug", "release" }

    filter "configurations:debug"
        defines { "SH_DEBUG", "_DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "SH_RELEASE", "NDEBUG" }
        optimize "Full"

    filter {}

    -- Platforms (i.e. win64, etc.)
    platforms { "win64" }

    filter "platforms:win64"
        system "Windows"
        architecture "x86_64"

    filter {}

    startproject "GraphicsCookbook"


project "GraphicsCookbook"
    kind "ConsoleApp"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    staticruntime "On"

    -- Language
    language "C++"
    cppdialect "C++20"
    exceptionhandling "Off"

    -- Dependencies
    dependson { "Extern" }

    defines {
        -- "LOG_USE_COLOR" -- Enable coloured output in logs.
    }

    -- Linker
    links { "Extern" }

    -- Output name
    targetname "graphics_cookbook"

    filter { "configurations:debug" }
        targetsuffix "_d"
    filter {}

    -- Directories
    targetdir "bin/"
    debugdir "%{cfg.targetdir}"

    libdirs {}

    uses { "Extern" }

    -- Include dirs
    includedirs {
        "code/",
    }

    -- Source files
    files {
        "code/**.h",
        "code/**.cpp",
        "code/**.c",
        "code/**.hlsl",
    }

    -- Copy all HLSL shader files to the target directory
    filter "files:code/**.hlsl"
        buildmessage 'Copying HLSL shader "%{file.relpath}" to "%{cfg.targetdir}/shaders/%{file.name}"'
        buildcommands {
            '{COPY} "%{file.abspath}" "%{cfg.targetdir}/shaders/"'
        }
        buildoutputs {
            "%{cfg.targetdir}/shaders/%{file.name}"
        }
    filter {}

    -- VS solution filters
    vpaths {
        ["*"] = "code/", -- Automatically mirror the code folder structure
    }


project "Extern"
    kind "StaticLib"
    staticruntime "On"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    files {
        "extern/assimp/include/**.h", -- Assimp *.h
        "extern/assimp/include/**.hpp", -- Assimp *.hpp
        "extern/assimp/include/**.inl", -- Assimp *.inl
        "extern/glm/**.hpp", -- glm
        "extern/dearimgui/**.h", -- Dear Imgui *.h
        "extern/dearimgui/**.cpp", -- Dear Imgui *.cpp
        "extern/sokol/**.h", -- sokol
        "extern/stb/**.h", -- stb libraries
    }

    removefiles {
        "extern/dearimgui/imgui/backends/**", -- Don't need imgui backends since we're using Sokol.
        "extern/dearimgui/imgui/examples/**", -- Don't need this I think.
        "extern/dearimgui/imgui/misc/**", -- Getting compilation errors. Do I need these? Bring this back if needed.
    }

    usage "PUBLIC"
        includedirs {
            "extern/assimp/include/",
            "extern/sokol/",
            "extern/glm/",
            "extern/stb/",
            "extern/dearimgui/",
            "extern/dearimgui/imgui/",
        }
        libdirs {
            "extern/assimp/lib/x64"
        }
        links {
            "assimp-vc143-mt"
        }


-- A project to store non-code repository files. e.g. this Premake script, images, etc.
project "Other"
    kind "None"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    files {
        "premake5.lua",
        ".gitignore",
    }
