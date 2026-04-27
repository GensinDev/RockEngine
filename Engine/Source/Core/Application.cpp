#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstring>
#include <iostream>
#include <string>
#include <filesystem>

namespace RockEngine
{
    Application::Application(const std::string &name)
        : m_EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)
    {
        std::cout << "Application created: " << name << std::endl;

        // Создаем окно приложения.
        m_Window = std::make_unique<EngineWindow>(name, 1280, 720);

        // Инициализируем рендерер.
        Renderer::Init();

        // Создаем framebuffer для Viewport.
        m_Framebuffer = std::make_unique<Framebuffer>(1280, 720);

        // Framebuffer для выбора
        m_PickingFramebuffer = std::make_unique<Framebuffer>(1280, 720);

        // Создаем стандартную сцену.
        m_Scene.CreateDefaultScene();
        m_CurrentAssetPath = std::filesystem::path(ROCKENGINE_PROJECT_DIR) / "Assets";

        // Инициализация ImGui.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    Application::~Application()
    {
        // Завершаем ImGui.
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Освобождаем ресурсы рендера.
        Renderer::Shutdown();

        std::cout << "Application destroyed" << std::endl;
    }

    void Application::CreateEntity(const std::string &name, EntityType type)
    {
        Entity entity;

        entity.Name = name;
        entity.Type = type;

        m_Scene.Entities.push_back(entity);
    }

    void Application::Run()
    {
        float lastTime = static_cast<float>(glfwGetTime());

        while (m_Running)
        {
            // Считаем deltaTime — время между кадрами.
            float currentTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            if (m_Window->ShouldClose())
            {
                m_Running = false;
            }

            // Очищаем главное окно редактора.
            glViewport(0, 0, 1280, 720);
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Начинаем новый кадр ImGui.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Главное docking-окно.
            ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_MenuBar |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus;

            const ImGuiViewport *viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGui::Begin("DockSpace Window", nullptr, windowFlags);

            ImGui::PopStyleVar(2);

            ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

            // =========================
            // TOP MENU
            // =========================
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New Scene"))
                    {
                        // Создаем новую сцену в памяти.
                        m_Scene.CreateDefaultScene();

                        // Сбрасываем выбранный объект.
                        m_SelectedEntity = -1;

                        // Сцена новая, значит она пока не привязана к файлу.
                        m_CurrentScenePath.clear();
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Save Scene"))
                    {
                        // Если текущая сцена уже связана с файлом,
                        // сохраняем именно в этот файл.
                        if (!m_CurrentScenePath.empty())
                        {
                            m_Scene.SaveToFile(m_CurrentScenePath.string());
                        }
                        else
                        {
                            // Если сцена новая и файла еще нет,
                            // временно сохраняем как Main.rockscene.
                            m_CurrentScenePath =
                                std::filesystem::path(ROCKENGINE_PROJECT_DIR) /
                                "Assets" /
                                "Scenes" /
                                "Main.rockscene";

                            m_Scene.SaveToFile(m_CurrentScenePath.string());
                        }
                    }

                    if (ImGui::MenuItem("Load Scene"))
                    {
                        m_CurrentScenePath =
                            std::filesystem::path(ROCKENGINE_PROJECT_DIR) /
                            "Assets" /
                            "Scenes" /
                            "Main.rockscene";

                        m_Scene.LoadFromFile(m_CurrentScenePath.string());

                        // После загрузки сбрасываем выбор,
                        // потому что старый индекс может указывать на другой объект.
                        m_SelectedEntity = -1;
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit"))
                    {
                        m_Running = false;
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Create"))
                {
                    if (ImGui::MenuItem("Cube"))
                    {
                        CreateEntity("Cube", EntityType::Mesh);
                    }

                    if (ImGui::MenuItem("Camera"))
                    {
                        CreateEntity("Camera", EntityType::Camera);
                    }

                    if (ImGui::MenuItem("Light"))
                    {
                        CreateEntity("Light", EntityType::Light);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            // =========================
            // HIERARCHY
            // =========================
            ImGui::Begin("Hierarchy");

            if (ImGui::Button("Create Entity"))
            {
                CreateEntity("New Entity", EntityType::Mesh);
            }

            ImGui::Separator();

            for (int i = 0; i < m_Scene.Entities.size(); i++)
            {
                auto &entity = m_Scene.Entities[i];

                std::string displayName =
                    entity.Name.empty() ? "Unnamed Entity" : entity.Name;

                // ##i — скрытый ID для ImGui.
                displayName += "##" + std::to_string(i);

                if (ImGui::Selectable(displayName.c_str(), m_SelectedEntity == i))
                {
                    m_SelectedEntity = i;
                }
            }

            ImGui::End();

            // Защита от неправильного индекса после удаления.
            if (m_SelectedEntity >= m_Scene.Entities.size())
            {
                m_SelectedEntity = -1;
            }

            // =========================
            // INSPECTOR
            // =========================
            ImGui::Begin("Inspector");

            if (m_SelectedEntity != -1)
            {
                auto &entity = m_Scene.Entities[m_SelectedEntity];

                static char nameBuffer[256] = "";
                static int lastSelectedEntity = -1;

                if (lastSelectedEntity != m_SelectedEntity)
                {
                    snprintf(nameBuffer, sizeof(nameBuffer), "%s", entity.Name.c_str());
                    lastSelectedEntity = m_SelectedEntity;
                }

                if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
                {
                    entity.Name = nameBuffer;
                }

                const char *typeName = "Mesh";

                if (entity.Type == EntityType::Camera)
                    typeName = "Camera";
                else if (entity.Type == EntityType::Light)
                    typeName = "Light";

                ImGui::Text("Type: %s", typeName);

                ImGui::Separator();

                ImGui::Text("Transform");
                ImGui::InputFloat3("Position", entity.TransformComponent.Position);
                ImGui::InputFloat3("Rotation", entity.TransformComponent.Rotation);
                ImGui::InputFloat3("Scale", entity.TransformComponent.Scale);

                ImGui::Separator();

                if (ImGui::Button("Delete Entity"))
                {
                    m_Scene.Entities.erase(
                        m_Scene.Entities.begin() + m_SelectedEntity);

                    m_SelectedEntity = -1;
                }
            }
            else
            {
                ImGui::Text("No entity selected");
            }

            ImGui::End();

            // =========================
            // ASSET BROWSER
            // =========================
            // Это окно показывает содержимое папки Assets.
            // Здесь мы можем:
            // - смотреть папки и файлы
            // - заходить в папки двойным кликом
            // - возвращаться назад
            // - выбирать ассеты
            // - загружать .rockscene двойным кликом
            // - перетаскивать файлы drag & drop
            ImGui::Begin("Assets");

            // Показываем текущую сцену.
            // Если путь пустой — сцена еще не сохранена как файл.
            if (!m_CurrentScenePath.empty())
            {
                ImGui::Text(
                    "Current Scene: %s",
                    m_CurrentScenePath.filename().string().c_str());
            }
            else
            {
                ImGui::Text("Current Scene: Untitled");
            }

            // Показываем текущую открытую папку в Asset Browser.
            ImGui::Text(
                "Path: %s",
                m_CurrentAssetPath.string().c_str());

            // Корневая папка Assets.
            // Выше нее кнопка Back подниматься не должна.
            std::filesystem::path assetsRoot =
                std::filesystem::path(ROCKENGINE_PROJECT_DIR) / "Assets";

            // Если мы не в корне Assets — показываем кнопку Back.
            if (m_CurrentAssetPath != assetsRoot)
            {
                if (ImGui::Button("<- Back"))
                {
                    // parent_path() возвращает папку выше.
                    m_CurrentAssetPath = m_CurrentAssetPath.parent_path();

                    // Сбрасываем выбранный ассет, чтобы не показывать старый файл.
                    m_SelectedAssetPath.clear();
                }
            }

            ImGui::Separator();

            // Проверяем, существует ли текущая папка.
            if (std::filesystem::exists(m_CurrentAssetPath))
            {
                // Проходим по всем файлам и папкам внутри текущей директории.
                for (const auto &entry : std::filesystem::directory_iterator(m_CurrentAssetPath))
                {
                    const std::filesystem::path path = entry.path();

                    // Имя файла или папки без полного пути.
                    std::string filename = path.filename().string();

                    // Очень важная строка.
                    // PushID дает каждому элементу уникальный внутренний ID.
                    // Даже если на экране два элемента имеют одинаковый текст,
                    // ImGui не будет путать их между собой.
                    ImGui::PushID(path.string().c_str());

                    if (entry.is_directory())
                    {
                        // Текст, который видит пользователь.
                        std::string label = "[Folder] " + filename;

                        // Проверяем, выбрана ли эта папка в Asset Browser.
                        bool isFolderSelected = (m_SelectedAssetPath == path);

                        // Selectable делает строку кликабельной.
                        if (ImGui::Selectable(label.c_str(), isFolderSelected))
                        {
                            // Одинарный клик выбирает папку.
                            m_SelectedAssetPath = path;
                        }

                        // Двойной клик по папке — заходим внутрь.
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            m_CurrentAssetPath = path;

                            // После входа в папку сбрасываем выбранный ассет.
                            m_SelectedAssetPath.clear();
                        }
                    }
                    else
                    {
                        // Текст, который видит пользователь.
                        std::string label = "[File] " + filename;

                        // Проверяем, выбран ли этот файл.
                        bool isFileSelected = (m_SelectedAssetPath == path);

                        // Одинарный клик выбирает файл.
                        if (ImGui::Selectable(label.c_str(), isFileSelected))
                        {
                            m_SelectedAssetPath = path;
                        }

                        // =========================
                        // DOUBLE CLICK LOAD
                        // =========================
                        // Если дважды кликнули по .rockscene — загружаем сцену.
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (path.extension() == ".rockscene")
                            {
                                // Запоминаем текущую открытую сцену.
                                m_CurrentScenePath = path;

                                // Загружаем сцену из файла.
                                m_Scene.LoadFromFile(m_CurrentScenePath.string());

                                // Сбрасываем выбранный объект,
                                // потому что после загрузки старый индекс может быть неверным.
                                m_SelectedEntity = -1;
                            }
                        }

                        // =========================
                        // DRAG SOURCE
                        // =========================
                        // Если пользователь начал тащить файл мышкой —
                        // создаем payload для drag & drop.
                        if (ImGui::BeginDragDropSource())
                        {
                            // Передаем полный путь к файлу.
                            std::string fullPath = path.string();

                            ImGui::SetDragDropPayload(
                                "ASSET_FILE",
                                fullPath.c_str(),
                                fullPath.size() + 1);

                            // Текст рядом с курсором во время перетаскивания.
                            ImGui::Text("%s", filename.c_str());

                            ImGui::EndDragDropSource();
                        }
                    }

                    // Закрываем уникальный ID элемента.
                    // PushID и PopID всегда должны идти парой.
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::Text("Folder not found");
            }

            ImGui::End();

            // =========================
            // ASSET INSPECTOR
            // =========================
            // Это окно показывает информацию о выбранном ассете.
            ImGui::Begin("Asset Inspector");

            if (!m_SelectedAssetPath.empty())
            {
                ImGui::Text(
                    "Name: %s",
                    m_SelectedAssetPath.filename().string().c_str());

                ImGui::Text(
                    "Path: %s",
                    m_SelectedAssetPath.string().c_str());

                if (std::filesystem::is_directory(m_SelectedAssetPath))
                {
                    ImGui::Text("Type: Folder");
                }
                else
                {
                    ImGui::Text("Type: File");

                    ImGui::Text(
                        "Extension: %s",
                        m_SelectedAssetPath.extension().string().c_str());

                    if (std::filesystem::exists(m_SelectedAssetPath))
                    {
                        auto fileSize = std::filesystem::file_size(m_SelectedAssetPath);

                        ImGui::Text(
                            "Size: %llu bytes",
                            static_cast<unsigned long long>(fileSize));
                    }
                }
            }
            else
            {
                ImGui::Text("No asset selected");
            }

            ImGui::End();

            // =========================
            // VIEWPORT
            // =========================
            ImGui::Begin("Viewport");

            ImVec2 viewportSize = ImGui::GetContentRegionAvail();

            if (viewportSize.x > 0 && viewportSize.y > 0)
            {
                m_Framebuffer->Resize(
                    static_cast<uint32_t>(viewportSize.x),
                    static_cast<uint32_t>(viewportSize.y));

                m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
                m_EditorCamera.OnUpdate(deltaTime);

                // =========================
                // 1. РЕНДЕР ОСНОВНОЙ СЦЕНЫ
                // =========================
                m_Framebuffer->Bind();

                glViewport(
                    0, 0,
                    static_cast<int>(viewportSize.x),
                    static_cast<int>(viewportSize.y));

                Renderer::BeginFrame();
                Renderer::RenderScene(m_Scene, m_EditorCamera, m_SelectedEntity);

                m_Framebuffer->Unbind();

                // Показываем сцену в ImGui
                ImGui::Image(
                    reinterpret_cast<void *>(
                        static_cast<intptr_t>(m_Framebuffer->GetColorAttachment())),
                    viewportSize,
                    ImVec2(0, 1),
                    ImVec2(1, 0));

                // =========================
                // VIEWPORT DROP TARGET
                // =========================
                // Viewport принимает файлы, которые мы перетаскиваем из Assets.
                if (ImGui::BeginDragDropTarget())
                {
                    // Проверяем, пришел ли payload типа "ASSET_FILE".
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_FILE"))
                    {
                        // payload->Data содержит строку с путем к файлу.
                        const char *droppedPath = static_cast<const char *>(payload->Data);

                        std::filesystem::path assetPath = droppedPath;

                        // Если в Viewport бросили .rockscene —
                        // загружаем эту сцену.
                        if (assetPath.extension() == ".rockscene")
                        {
                            m_CurrentScenePath = assetPath;
                            m_Scene.LoadFromFile(m_CurrentScenePath.string());
                            m_SelectedEntity = -1;
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                // =========================
                // 2. РЕНДЕР PICKING (СКРЫТЫЙ)
                // =========================
                m_PickingFramebuffer->Bind();

                glViewport(
                    0, 0,
                    static_cast<int>(viewportSize.x),
                    static_cast<int>(viewportSize.y));

                Renderer::BeginFrame();
                Renderer::RenderScenePicking(m_Scene, m_EditorCamera);

                m_PickingFramebuffer->Unbind();

                // =========================
                // 3. ЧТЕНИЕ КЛИКА МЫШИ
                // =========================
                if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
                {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 windowPos = ImGui::GetWindowPos();

                    int x = static_cast<int>(mousePos.x - windowPos.x);
                    int y = static_cast<int>(mousePos.y - windowPos.y);

                    m_PickingFramebuffer->Bind();

                    unsigned char pixel[4];

                    glReadPixels(
                        x,
                        static_cast<int>(viewportSize.y) - y,
                        1, 1,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        pixel);

                    m_PickingFramebuffer->Unbind();

                    int id = pixel[0];

                    if (id > 0)
                    {
                        m_SelectedEntity = id - 1;
                    }
                }
            }

            ImGui::End();

            // Закрываем DockSpace Window.
            ImGui::End();

            // Рисуем ImGui.
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            Renderer::EndFrame();

            // Обновляем окно: события + swap buffers.
            m_Window->OnUpdate();
        }
    }
}