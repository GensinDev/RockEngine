#pragma once

namespace RockEngine
{
    class Scene;
    class EditorCamera;

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

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
        static unsigned int s_VertexArray;
        static unsigned int s_VertexBuffer;
        static unsigned int s_ShaderProgram;
    };
}