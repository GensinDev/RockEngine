#pragma once

#include "Core/EngineWindow.h"
#include "Scene/Scene.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/EditorCamera.h"

#include <memory>
#include <string>
#include <filesystem>

namespace RockEngine
{
    class Application
    {
    public:
        Application(const std::string &name);
        ~Application();

        void Run();

    private:
        std::unique_ptr<EngineWindow> m_Window; // хранит объект и автоматически удаляет его когда Application уничтожается
        bool m_Running = true;

        Scene m_Scene;
        int m_SelectedEntity = -1;

        std::unique_ptr<Framebuffer> m_Framebuffer;

        EditorCamera m_EditorCamera;

        void CreateEntity(const std::string &name, EntityType type);

        std::unique_ptr<Framebuffer> m_PickingFramebuffer;

        std::filesystem::path m_CurrentAssetPath;

        // Путь к текущей открытой сцене.
        // Если путь пустой — сцена еще не сохранена как файл.
        std::filesystem::path m_CurrentScenePath;

        // Выбранный файл в Asset Browser.
        std::filesystem::path m_SelectedAssetPath;
    };
}