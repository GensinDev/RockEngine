#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
#include <iostream>
#include <string>

namespace RockEngine
{
    Application::Application(const std::string &name)
        : m_EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)
    {
        std::cout << "Application created: " << name << std::endl;

        // Создаем окно редактора.
        m_Window = std::make_unique<EngineWindow>(name, 1280, 720);

        // Инициализируем OpenGL renderer.
        Renderer::Init();

        // Основной framebuffer для отображения сцены в Viewport.
        m_Framebuffer = std::make_unique<Framebuffer>(1280, 720);

        // Скрытый framebuffer для выбора объектов мышкой.
        m_PickingFramebuffer = std::make_unique<Framebuffer>(1280, 720);

        // Создаем стартовую сцену: Camera + Cube + Light.
        m_Scene.CreateDefaultScene();

        // Начальная папка Asset Browser.
        m_CurrentAssetPath =
            std::filesystem::path(ROCKENGINE_PROJECT_DIR) / "Assets";

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

        // Освобождаем OpenGL ресурсы Renderer.
        Renderer::Shutdown();

        std::cout << "Application destroyed" << std::endl;
    }

    void Application::CreateEntity(const std::string &name)
    {
        // Создает пустой Entity только с Transform.
        Entity entity;
        entity.Name = name;

        m_Scene.Entities.push_back(entity);
    }

    void Application::CreateCube()
    {
        // MeshRenderer означает: этот Entity должен рисоваться Renderer-ом.
        Entity cube;
        cube.Name = "Cube";
        cube.HasMeshRenderer = true;

        m_Scene.Entities.push_back(cube);
    }

    void Application::CreateCamera()
    {
        // CameraComponent пока является маркером.
        // Позже из него сделаем игровую камеру.
        Entity camera;
        camera.Name = "Camera";
        camera.HasCamera = true;
        camera.Camera.Primary = false;
        camera.Transform.Position = {0.0f, 0.0f, 5.0f};

        m_Scene.Entities.push_back(camera);
    }

    void Application::CreateLight()
    {
        // LightComponent пока не влияет на освещение,
        // но уже хранится в Entity как отдельный компонент.
        Entity light;
        light.Name = "Light";
        light.HasLight = true;
        light.Transform.Position = {2.0f, 2.0f, 0.0f};

        m_Scene.Entities.push_back(light);
    }

    void Application::Run()
    {
        float lastTime = static_cast<float>(glfwGetTime());

        while (m_Running)
        {
            // deltaTime нужен для стабильного движения камеры.
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

            // Начинаем кадр ImGui.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // =========================
            // DOCKSPACE
            // =========================
            // Большое невидимое окно, в котором живут все панели редактора.
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
                        m_Scene.CreateDefaultScene();
                        m_SelectedEntity = -1;
                        m_CurrentScenePath.clear();
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Save Scene"))
                    {
                        if (!m_CurrentScenePath.empty())
                        {
                            m_Scene.SaveToFile(m_CurrentScenePath.string());
                        }
                        else
                        {
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
                    if (ImGui::MenuItem("Empty Entity"))
                    {
                        CreateEntity("Empty Entity");
                    }

                    if (ImGui::MenuItem("Cube"))
                    {
                        CreateCube();
                    }

                    if (ImGui::MenuItem("Camera"))
                    {
                        CreateCamera();
                    }

                    if (ImGui::MenuItem("Light"))
                    {
                        CreateLight();
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
                CreateEntity("New Entity");
            }

            ImGui::Separator();

            for (int i = 0; i < static_cast<int>(m_Scene.Entities.size()); i++)
            {
                auto &entity = m_Scene.Entities[i];

                std::string displayName =
                    entity.Name.empty() ? "Unnamed Entity" : entity.Name;

                // ##i — скрытый уникальный ID для ImGui.
                displayName += "##" + std::to_string(i);

                if (ImGui::Selectable(displayName.c_str(), m_SelectedEntity == i))
                {
                    m_SelectedEntity = i;
                }
            }

            ImGui::End();

            // Если объект удалили, индекс мог стать невалидным.
            if (m_SelectedEntity >= static_cast<int>(m_Scene.Entities.size()))
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

                // Буфер нужен, потому что ImGui::InputText работает с char[],
                // а имя Entity хранится как std::string.
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

                ImGui::Separator();

                // =========================
                // COMPONENTS
                // =========================
                ImGui::Text("Components");

                ImGui::Checkbox("Mesh Renderer", &entity.HasMeshRenderer);
                ImGui::Checkbox("Camera", &entity.HasCamera);
                ImGui::Checkbox("Light", &entity.HasLight);

                if (entity.HasMeshRenderer)
                {
                    ImGui::Separator();
                    ImGui::Text("Mesh Renderer Component");

                    // Включает/выключает отрисовку объекта.
                    ImGui::Checkbox("Enabled", &entity.MeshRenderer.Enabled);

                    // Меняет цвет объекта.
                    ImGui::ColorEdit3("Color", &entity.MeshRenderer.Color.x);
                }

                ImGui::Separator();

                // =========================
                // TRANSFORM COMPONENT
                // =========================
                ImGui::Text("Transform");

                // glm::vec3 хранит x/y/z подряд в памяти,
                // поэтому можно передавать адрес x.
                ImGui::DragFloat3("Position", &entity.Transform.Position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &entity.Transform.Rotation.x, 0.5f);
                ImGui::DragFloat3("Scale", &entity.Transform.Scale.x, 0.1f);

                if (entity.HasLight)
                {
                    ImGui::Separator();
                    ImGui::Text("Light Component");
                    ImGui::ColorEdit3("Color", &entity.Light.Color.x);
                    ImGui::DragFloat("Intensity", &entity.Light.Intensity, 0.1f, 0.0f, 100.0f);
                }

                if (entity.HasCamera)
                {
                    ImGui::Separator();
                    ImGui::Text("Camera Component");
                    ImGui::Checkbox("Primary", &entity.Camera.Primary);
                }

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
            ImGui::Begin("Assets");

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

            ImGui::Text(
                "Path: %s",
                m_CurrentAssetPath.string().c_str());

            std::filesystem::path assetsRoot =
                std::filesystem::path(ROCKENGINE_PROJECT_DIR) / "Assets";

            if (m_CurrentAssetPath != assetsRoot)
            {
                if (ImGui::Button("<- Back"))
                {
                    m_CurrentAssetPath = m_CurrentAssetPath.parent_path();
                    m_SelectedAssetPath.clear();
                }
            }

            ImGui::Separator();

            if (std::filesystem::exists(m_CurrentAssetPath))
            {
                for (const auto &entry : std::filesystem::directory_iterator(m_CurrentAssetPath))
                {
                    const std::filesystem::path path = entry.path();
                    std::string filename = path.filename().string();

                    // Уникальный ID, чтобы ImGui не путал элементы с одинаковым именем.
                    ImGui::PushID(path.string().c_str());

                    if (entry.is_directory())
                    {
                        std::string label = "[Folder] " + filename;

                        bool isFolderSelected = (m_SelectedAssetPath == path);

                        if (ImGui::Selectable(label.c_str(), isFolderSelected))
                        {
                            m_SelectedAssetPath = path;
                        }

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            m_CurrentAssetPath = path;
                            m_SelectedAssetPath.clear();
                        }
                    }
                    else
                    {
                        std::string label = "[File] " + filename;

                        bool isFileSelected = (m_SelectedAssetPath == path);

                        if (ImGui::Selectable(label.c_str(), isFileSelected))
                        {
                            m_SelectedAssetPath = path;
                        }

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (path.extension() == ".rockscene")
                            {
                                m_CurrentScenePath = path;
                                m_Scene.LoadFromFile(m_CurrentScenePath.string());
                                m_SelectedEntity = -1;
                            }
                        }

                        if (ImGui::BeginDragDropSource())
                        {
                            std::string fullPath = path.string();

                            ImGui::SetDragDropPayload(
                                "ASSET_FILE",
                                fullPath.c_str(),
                                fullPath.size() + 1);

                            ImGui::Text("%s", filename.c_str());

                            ImGui::EndDragDropSource();
                        }
                    }

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

                m_PickingFramebuffer->Resize(
                    static_cast<uint32_t>(viewportSize.x),
                    static_cast<uint32_t>(viewportSize.y));

                // Обновляем editor camera только когда Viewport активен.
                m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
                m_EditorCamera.OnUpdate(deltaTime);

                // Рендер обычной сцены.
                m_Framebuffer->Bind();

                glViewport(
                    0,
                    0,
                    static_cast<int>(viewportSize.x),
                    static_cast<int>(viewportSize.y));

                Renderer::BeginFrame();
                Renderer::RenderScene(m_Scene, m_EditorCamera, m_SelectedEntity);

                m_Framebuffer->Unbind();

                // Показываем результат framebuffer в ImGui.
                ImGui::Image(
                    reinterpret_cast<void *>(
                        static_cast<intptr_t>(m_Framebuffer->GetColorAttachment())),
                    viewportSize,
                    ImVec2(0, 1),
                    ImVec2(1, 0));

                // Рендер скрытого picking buffer.
                m_PickingFramebuffer->Bind();

                glViewport(
                    0,
                    0,
                    static_cast<int>(viewportSize.x),
                    static_cast<int>(viewportSize.y));

                Renderer::BeginFrame();
                Renderer::RenderScenePicking(m_Scene, m_EditorCamera);

                m_PickingFramebuffer->Unbind();

                // Drag & Drop файлов в Viewport.
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_FILE"))
                    {
                        const char *droppedPath =
                            static_cast<const char *>(payload->Data);

                        std::filesystem::path assetPath = droppedPath;

                        if (assetPath.extension() == ".rockscene")
                        {
                            m_CurrentScenePath = assetPath;
                            m_Scene.LoadFromFile(m_CurrentScenePath.string());
                            m_SelectedEntity = -1;
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                // Mouse picking.
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
                        1,
                        1,
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

            // Отрисовываем весь ImGui UI.
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            Renderer::EndFrame();

            // Обновляем окно: input events + swap buffers.
            m_Window->OnUpdate();
        }
    }
}