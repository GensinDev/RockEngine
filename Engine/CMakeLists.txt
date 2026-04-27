add_library(RockEngine
    Source/Core/Application.cpp
    Source/Core/Application.h
    Source/Core/EngineWindow.cpp
    Source/Core/EngineWindow.h

    Source/Renderer/Renderer.cpp
    Source/Renderer/Renderer.h
    Source/Renderer/Framebuffer.cpp
    Source/Renderer/Framebuffer.h
    Source/Renderer/EditorCamera.cpp
    Source/Renderer/EditorCamera.h
    Source/Renderer/Mesh.cpp
    Source/Renderer/Mesh.h

    Source/Scene/Scene.cpp
    Source/Scene/Scene.h
    Source/Scene/Entity.h
    Source/Scene/Components.h
)

target_include_directories(RockEngine
    PUBLIC Source
)

find_package(OpenGL REQUIRED)

target_link_libraries(RockEngine
    PUBLIC 
        glfw
        libglew_static
        OpenGL::GL
        imgui_lib
        glm::glm
)