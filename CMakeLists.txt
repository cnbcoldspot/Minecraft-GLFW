cmake_minimum_required(VERSION 3.16)

project(MinecraftGLFW
    VERSION 1.0.0
    DESCRIPTION "Voxel engine written in C++ and OpenGL"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Optional accelerators. Harmless when the tools are not installed.
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Minecraft GLFW")
set(SOURCE_DIR "${PROJECT_DIR}/Source")

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

set(SOURCES
    "${SOURCE_DIR}/main.cpp"

    "${SOURCE_DIR}/Graphic/Shader.cpp"
    "${SOURCE_DIR}/Graphic/Texture.cpp"
    "${SOURCE_DIR}/Graphic/GUI/GUIElement.cpp"
    "${SOURCE_DIR}/Graphic/GUI/GUIMesh.cpp"
    "${SOURCE_DIR}/Graphic/GUI/GUIElements/Crosshair.cpp"

    "${SOURCE_DIR}/Inventory/Inventory.cpp"
    "${SOURCE_DIR}/Inventory/Item.cpp"
    "${SOURCE_DIR}/Inventory/ItemStack.cpp"

    "${SOURCE_DIR}/Mesh/BigMesh.cpp"
    "${SOURCE_DIR}/Mesh/Mesh.cpp"
    "${SOURCE_DIR}/Mesh/ShardMesh.cpp"

    "${SOURCE_DIR}/Models/Model.cpp"

    "${SOURCE_DIR}/Physics/AABB.cpp"

    "${SOURCE_DIR}/Player/Player.cpp"
    "${SOURCE_DIR}/Player/PlayerGUI.cpp"

    "${SOURCE_DIR}/Utility/Mexception.cpp"

    "${SOURCE_DIR}/Vertex/IndexBuffer.cpp"
    "${SOURCE_DIR}/Vertex/VertexArray.cpp"
    "${SOURCE_DIR}/Vertex/VertexBuffer.cpp"

    "${SOURCE_DIR}/Window/Camera.cpp"
    "${SOURCE_DIR}/Window/Events.cpp"
    "${SOURCE_DIR}/Window/Window.cpp"

    "${SOURCE_DIR}/World/Chunk.cpp"
    "${SOURCE_DIR}/World/World.cpp"
    "${SOURCE_DIR}/World/Container/Neighbor.cpp"
    "${SOURCE_DIR}/World/Voxel/Voxel.cpp"
    "${SOURCE_DIR}/World/Voxel/VoxelData.cpp"
    "${SOURCE_DIR}/World/Voxel/VoxelDataBase.cpp"
)

add_executable(MinecraftGLFW ${SOURCES})

target_include_directories(MinecraftGLFW PRIVATE
    "${SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/GLM"     # header-only, vendored (0.9.9.7)
    "${CMAKE_CURRENT_SOURCE_DIR}/Noise"   # FastNoiseLite, header-only
    "${CMAKE_CURRENT_SOURCE_DIR}/STB"     # stb_image, header-only
)

# GLEW is linked statically on Windows; the headers need to know.
target_compile_definitions(MinecraftGLFW PRIVATE GLEW_STATIC)

# ---------------------------------------------------------------------------
# Dependencies
#
# Linux/macOS: OpenGL, GLEW and GLFW come from the system package manager.
# Windows:     the prebuilt x64 libraries vendored in this repository since
#              2020 are used, so the Visual Studio path keeps working as-is.
# ---------------------------------------------------------------------------

find_package(OpenGL REQUIRED)
target_link_libraries(MinecraftGLFW PRIVATE OpenGL::GL)

if(WIN32)
    set(VENDOR_ARCH "x65")  # the repo's (misspelled) name for the x64 libraries
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(VENDOR_ARCH "x32")
    endif()

    target_include_directories(MinecraftGLFW PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/GLEW/${VENDOR_ARCH}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/GLFW/${VENDOR_ARCH}/include")

    target_link_libraries(MinecraftGLFW PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/GLEW/${VENDOR_ARCH}/lib/glew32s.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/GLFW/${VENDOR_ARCH}/lib/glfw3.lib")

    if(MSVC)
        # Mirrors the #pragma comment(linker, "/STACK:...") in main.cpp, kept
        # from the original 2020 project. Nothing measured needs it.
        target_link_options(MinecraftGLFW PRIVATE "/STACK:67108864")
    endif()
else()
    find_package(GLEW REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(MinecraftGLFW PRIVATE
        GLEW::GLEW
        glfw
        Threads::Threads)

    # No stack-size link option here: on Linux the main thread's stack comes
    # from RLIMIT_STACK, and GNU ld silently ignores -z stacksize for ELF.
    # Nothing in the engine needs more than the default (see main.cpp).
endif()

# ---------------------------------------------------------------------------
# Resources
#
# main.cpp opens "Resource/Shader/..." relative to the working directory, so
# the resource tree is mirrored next to the executable.
# ---------------------------------------------------------------------------

add_custom_command(TARGET MinecraftGLFW POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_DIR}/Resource"
        "$<TARGET_FILE_DIR:MinecraftGLFW>/Resource"
    COMMENT "Copying Resource/ next to the executable")

if(MSVC)
    set_target_properties(MinecraftGLFW PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MinecraftGLFW>")
endif()
