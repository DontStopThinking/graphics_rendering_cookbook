workspace "GraphicsCookbook"
    -- project files output location
    location "projects/%{_ACTION}/"

    filename "%{wks.name:lower()}"

    -- Unity build
    enableunitybuild "On"

    -- Global defines
    defines { "_CRT_SECURE_NO_WARNINGS" }

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


project "Extern"
    kind "None"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    files {
        "extern/glm/**.hpp", -- glm
        "extern/sokol/**.h", -- sokol
        "extern/stb/**.h", -- stb libraries
    }

    usage "PUBLIC"
        includedirs {
            "extern/sokol/",
            "extern/glm/",
            "extern/stb/",
        }


project "GraphicsCookbook"
    kind "ConsoleApp"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    staticruntime "On"

    -- Language
    language "C++"
    cppdialect "C++20"
    exceptionhandling "Off"

    defines {
        -- "LOG_USE_COLOR" -- Enable coloured output in logs.
    }

    -- Linker
    links {}

    -- Output name
    targetname "graphics_cookbook"

    filter { "configurations:debug" }
        targetsuffix "_d"
    filter {}

    -- Directories
    targetdir "bin/"
    objdir "tmp/%{cfg.buildcfg}/%{prj.name:lower()}"
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


-- A project to store non-code repository files. e.g. this Premake script, images, etc.
project "Other"
    kind "None"

    location "%{wks.location}/%{prj.name:lower()}"
    filename "%{prj.name:lower()}"

    files {
        "premake5.lua",
        ".gitignore",
    }
