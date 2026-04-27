add_library(RockEngine
    Source/Core/Application.cpp
    Source/Core/EngineWindow.cpp

    Source/Scene/Scene.cpp
    Source/Scene/Scene.h
    Source/Scene/Entity.h
    Source/Scene/Transform.h

    Source/Renderer/Renderer.cpp
    Source/Renderer/Renderer.h
    Source/Renderer/Framebuffer.cpp
    Source/Renderer/Framebuffer.h
    Source/Renderer/EditorCamera.cpp
    Source/Renderer/EditorCamera.h
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