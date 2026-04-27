#pragma once

#include "Core/EngineWindow.h"

#include "Renderer/Framebuffer.h"
#include "Renderer/EditorCamera.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include <filesystem>
#include <memory>
#include <string>

namespace RockEngine
{
    class Application
    {
    public:
        Application(const std::string &name);
        ~Application();

        void Run();

    private:
        // Создает обычный объект сцены.
        // Компоненты задаются отдельными bool-полями:
        // HasMeshRenderer / HasCamera / HasLight.
        void CreateEntity(const std::string &name);

        // Создает куб, то есть Entity с MeshRenderer.
        void CreateCube();

        // Создает Entity с CameraComponent.
        void CreateCamera();

        // Создает Entity с LightComponent.
        void CreateLight();

    private:
        // Главное окно редактора.
        std::unique_ptr<EngineWindow> m_Window;

        // Framebuffer для обычного Viewport-рендера.
        std::unique_ptr<Framebuffer> m_Framebuffer;

        // Framebuffer для GPU picking.
        // Он скрытый: пользователь его не видит.
        std::unique_ptr<Framebuffer> m_PickingFramebuffer;

        // Главная сцена.
        Scene m_Scene;

        // Камера редактора.
        EditorCamera m_EditorCamera;

        // Индекс выбранного Entity.
        // -1 значит ничего не выбрано.
        int m_SelectedEntity = -1;

        // Флаг работы приложения.
        bool m_Running = true;

        // Текущая папка в Asset Browser.
        std::filesystem::path m_CurrentAssetPath;

        // Текущий выбранный ассет.
        std::filesystem::path m_SelectedAssetPath;

        // Текущий открытый файл сцены.
        std::filesystem::path m_CurrentScenePath;
    };
}