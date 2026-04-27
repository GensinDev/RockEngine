#pragma once

#include <memory>

namespace RockEngine
{
    class Scene;
    class EditorCamera;
    class Mesh;

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static std::shared_ptr<Mesh> GetCubeMesh();

        // Основной рендер сцены.
        static void RenderScene(
            const Scene &scene,
            const EditorCamera &camera,
            int selectedEntity);

        // Скрытый рендер для выбора объекта мышкой.
        static void RenderScenePicking(
            const Scene &scene,
            const EditorCamera &camera);

    private:
        static unsigned int s_ShaderProgram;
    };
}