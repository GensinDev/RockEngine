#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Mesh.h"

#include "Scene/Scene.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <vector>

namespace RockEngine
{
    // =========================
    // SHADER PROGRAM
    // =========================
    // Теперь Renderer больше не хранит VAO/VBO куба напрямую.
    // Геометрия переехала в Mesh.
    //
    // Renderer хранит только shader program,
    // потому что один шейдер может рисовать разные Mesh.
    unsigned int Renderer::s_ShaderProgram = 0;

    // =========================
    // BUILT-IN MESHES
    // =========================
    // Это встроенный куб движка.
    // Пока он создается прямо в Renderer::Init().
    //
    // Позже мы вынесем это в AssetManager или MeshLibrary.
    static std::shared_ptr<Mesh> s_CubeMesh;

    void Renderer::Init()
    {
        // =========================
        // OPENGL STATE
        // =========================

        // Depth Test нужен для 3D.
        // Он делает так, что ближние объекты перекрывают дальние.
        glEnable(GL_DEPTH_TEST);

        // Back-face culling отключает отрисовку задних граней.
        // Это ускоряет рендер и делает куб визуально чище.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // =========================
        // SHADERS
        // =========================
        // Vertex Shader:
        // - принимает позицию вершины
        // - принимает цвет вершины
        // - умножает позицию на MVP матрицу
        //
        // MVP = Projection * View * Model
        //
        // Model      — положение объекта в мире
        // View       — камера
        // Projection — перспектива
        const char *vertexSrc = R"(
#version 330

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 u_MVP;

void main()
{
    vColor = aColor;
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
)";

        // Fragment Shader:
        // - либо использует цвет вершины
        // - либо принудительный цвет объекта через u_Color
        //
        // u_UseObjectColor нужен для:
        // - подсветки выбранного объекта
        // - GPU picking
        // - цвета MeshRendererComponent
        const char *fragmentSrc = R"(
#version 330

in vec3 vColor;

uniform vec3 u_Color;
uniform int u_UseObjectColor;

out vec4 FragColor;

void main()
{
    if (u_UseObjectColor == 1)
    {
        FragColor = vec4(u_Color, 1.0);
    }
    else
    {
        FragColor = vec4(vColor, 1.0);
    }
}
)";

        int success;
        char infoLog[512];

        // =========================
        // VERTEX SHADER COMPILE
        // =========================

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(
            vertexShader,
            1,
            &vertexSrc,
            nullptr);

        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "Vertex Shader Error:\n"
                      << infoLog << std::endl;
        }

        // =========================
        // FRAGMENT SHADER COMPILE
        // =========================

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(
            fragmentShader,
            1,
            &fragmentSrc,
            nullptr);

        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "Fragment Shader Error:\n"
                      << infoLog << std::endl;
        }

        // =========================
        // SHADER PROGRAM LINK
        // =========================

        s_ShaderProgram = glCreateProgram();

        glAttachShader(s_ShaderProgram, vertexShader);
        glAttachShader(s_ShaderProgram, fragmentShader);

        glLinkProgram(s_ShaderProgram);

        glGetProgramiv(s_ShaderProgram, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(s_ShaderProgram, 512, nullptr, infoLog);
            std::cout << "Shader Link Error:\n"
                      << infoLog << std::endl;
        }

        // После линковки отдельные shader objects больше не нужны.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // =========================
        // CREATE BUILT-IN CUBE MESH
        // =========================
        // Теперь куб — это не VAO/VBO внутри Renderer.
        // Куб — это объект Mesh.
        //
        // Vertex содержит:
        // - Position
        // - Color
        //
        // 6 граней * 2 треугольника * 3 вершины = 36 вершин.
        std::vector<Vertex> cubeVertices =
            {
                // Front face - red
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

                // Back face - green
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

                {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

                // Left face - blue
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},

                {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

                // Right face - yellow
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
                {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},

                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
                {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
                {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},

                // Top face - cyan
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
                {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},

                {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
                {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},

                // Bottom face - magenta
                {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},

                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
                {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
                {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
            };

        // Создаем Mesh из массива вершин.
        // Mesh сам создаст VAO/VBO внутри себя.
        s_CubeMesh = std::make_shared<Mesh>(cubeVertices);
    }

    void Renderer::Shutdown()
    {
        // Освобождаем shader program.
        glDeleteProgram(s_ShaderProgram);

        // Освобождаем shared_ptr.
        // Когда shared_ptr станет пустым,
        // Mesh должен освободить свои OpenGL ресурсы.
        s_CubeMesh.reset();
    }

    void Renderer::BeginFrame()
    {
        // Цвет очистки viewport.
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);

        // Очищаем цвет и depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::EndFrame()
    {
        // Пока пусто.
        // SwapBuffers делает EngineWindow.
    }

    std::shared_ptr<Mesh> Renderer::GetCubeMesh()
    {
        return s_CubeMesh;
    }

    void Renderer::RenderScene(
        const Scene &scene,
        const EditorCamera &camera,
        int selectedEntity)
    {
        // Активируем shader program.
        glUseProgram(s_ShaderProgram);

        // На всякий случай гарантируем обычный fill режим.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

        // Получаем locations uniform-переменных один раз перед циклом.
        int mvpLocation = glGetUniformLocation(s_ShaderProgram, "u_MVP");
        int colorLocation = glGetUniformLocation(s_ShaderProgram, "u_Color");
        int useObjectColorLocation =
            glGetUniformLocation(s_ShaderProgram, "u_UseObjectColor");

        for (int i = 0; i < static_cast<int>(scene.Entities.size()); i++)
        {
            const auto &entity = scene.Entities[i];

            // ECS-lite:
            // объект рисуется только если у него есть MeshRenderer
            // и этот компонент включен.
            if (!entity.HasMeshRenderer || !entity.MeshRenderer.Enabled)
            {
                continue;
            }

            // Если MeshRenderer не имеет Mesh,
            // рисовать нечего.
            if (!entity.MeshRenderer.MeshPtr)
            {
                continue;
            }

            // Model matrix берется из TransformComponent.
            glm::mat4 model = entity.Transform.GetTransform();

            // Итоговая матрица для шейдера.
            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(
                mvpLocation,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

            if (i == selectedEntity)
            {
                // Выбранный объект рисуем оранжевым.
                glUniform1i(useObjectColorLocation, 1);
                glUniform3f(colorLocation, 1.0f, 0.6f, 0.1f);
            }
            else
            {
                // Обычный объект рисуем цветом из MeshRenderer.
                glUniform1i(useObjectColorLocation, 1);
                glUniform3f(
                    colorLocation,
                    entity.MeshRenderer.Color.x,
                    entity.MeshRenderer.Color.y,
                    entity.MeshRenderer.Color.z);
            }

            // Mesh сам знает, какой VAO нужно привязать.
            entity.MeshRenderer.MeshPtr->Bind();

            // Mesh сам знает, сколько вершин нужно нарисовать.
            entity.MeshRenderer.MeshPtr->Draw();
        }

        glUseProgram(0);
    }

    void Renderer::RenderScenePicking(
        const Scene &scene,
        const EditorCamera &camera)
    {
        // Picking — это скрытый рендер.
        // Мы рисуем каждый объект уникальным цветом,
        // чтобы потом прочитать пиксель под мышкой.
        glUseProgram(s_ShaderProgram);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

        int mvpLocation = glGetUniformLocation(s_ShaderProgram, "u_MVP");
        int colorLocation = glGetUniformLocation(s_ShaderProgram, "u_Color");
        int useObjectColorLocation =
            glGetUniformLocation(s_ShaderProgram, "u_UseObjectColor");

        // В picking всегда используем принудительный цвет объекта.
        glUniform1i(useObjectColorLocation, 1);

        for (int i = 0; i < static_cast<int>(scene.Entities.size()); i++)
        {
            const auto &entity = scene.Entities[i];

            // Picking нужен только для реально рисуемых Mesh объектов.
            if (!entity.HasMeshRenderer || !entity.MeshRenderer.Enabled)
            {
                continue;
            }

            if (!entity.MeshRenderer.MeshPtr)
            {
                continue;
            }

            glm::mat4 model = entity.Transform.GetTransform();
            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(
                mvpLocation,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

            // Кодируем ID объекта в красный канал.
            // 0 = ничего.
            // Поэтому используем i + 1.
            float idColor = static_cast<float>(i + 1) / 255.0f;

            glUniform3f(colorLocation, idColor, 0.0f, 0.0f);

            entity.MeshRenderer.MeshPtr->Bind();
            entity.MeshRenderer.MeshPtr->Draw();
        }

        glUseProgram(0);
    }
}