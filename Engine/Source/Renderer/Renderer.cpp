#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"

#include "Scene/Scene.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace RockEngine
{
    // VAO хранит описание формата вершин:
    // какие данные есть у вершины и как OpenGL должен их читать.
    unsigned int Renderer::s_VertexArray = 0;

    // VBO хранит сами вершины куба.
    unsigned int Renderer::s_VertexBuffer = 0;

    // Shader program = vertex shader + fragment shader.
    unsigned int Renderer::s_ShaderProgram = 0;

    void Renderer::Init()
    {
        // Depth test нужен для 3D.
        // Ближние пиксели будут перекрывать дальние.
        glEnable(GL_DEPTH_TEST);

        // Отсекаем задние грани куба.
        // Это ускоряет рендер и убирает лишнюю геометрию.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Формат вершины:
        // x, y, z, r, g, b
        //
        // x y z — позиция вершины
        // r g b — цвет вершины
        float vertices[] =
            {
                // Front face - red
                -0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,
                0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,
                0.5f,
                0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,

                0.5f,
                0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,
                -0.5f,
                0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,
                -0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                0.0f,

                // Back face - green
                -0.5f,
                -0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,
                -0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,
                0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,

                0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,
                0.5f,
                -0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,
                -0.5f,
                -0.5f,
                -0.5f,
                0.0f,
                1.0f,
                0.0f,

                // Left face - blue
                -0.5f,
                0.5f,
                0.5f,
                0.0f,
                0.0f,
                1.0f,
                -0.5f,
                0.5f,
                -0.5f,
                0.0f,
                0.0f,
                1.0f,
                -0.5f,
                -0.5f,
                -0.5f,
                0.0f,
                0.0f,
                1.0f,

                -0.5f,
                -0.5f,
                -0.5f,
                0.0f,
                0.0f,
                1.0f,
                -0.5f,
                -0.5f,
                0.5f,
                0.0f,
                0.0f,
                1.0f,
                -0.5f,
                0.5f,
                0.5f,
                0.0f,
                0.0f,
                1.0f,

                // Right face - yellow
                0.5f,
                0.5f,
                0.5f,
                1.0f,
                1.0f,
                0.0f,
                0.5f,
                -0.5f,
                -0.5f,
                1.0f,
                1.0f,
                0.0f,
                0.5f,
                0.5f,
                -0.5f,
                1.0f,
                1.0f,
                0.0f,

                0.5f,
                -0.5f,
                -0.5f,
                1.0f,
                1.0f,
                0.0f,
                0.5f,
                0.5f,
                0.5f,
                1.0f,
                1.0f,
                0.0f,
                0.5f,
                -0.5f,
                0.5f,
                1.0f,
                1.0f,
                0.0f,

                // Top face - cyan
                -0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                1.0f,
                -0.5f,
                0.5f,
                0.5f,
                0.0f,
                1.0f,
                1.0f,
                0.5f,
                0.5f,
                0.5f,
                0.0f,
                1.0f,
                1.0f,

                0.5f,
                0.5f,
                0.5f,
                0.0f,
                1.0f,
                1.0f,
                0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                1.0f,
                -0.5f,
                0.5f,
                -0.5f,
                0.0f,
                1.0f,
                1.0f,

                // Bottom face - magenta
                -0.5f,
                -0.5f,
                -0.5f,
                1.0f,
                0.0f,
                1.0f,
                0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                1.0f,
                -0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                1.0f,

                0.5f,
                -0.5f,
                0.5f,
                1.0f,
                0.0f,
                1.0f,
                -0.5f,
                -0.5f,
                -0.5f,
                1.0f,
                0.0f,
                1.0f,
                0.5f,
                -0.5f,
                -0.5f,
                1.0f,
                0.0f,
                1.0f,
            };

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

        // Создаем и компилируем vertex shader.
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "Vertex Shader Error:\n"
                      << infoLog << std::endl;
        }

        // Создаем и компилируем fragment shader.
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "Fragment Shader Error:\n"
                      << infoLog << std::endl;
        }

        // Создаем shader program и линкуем оба шейдера.
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

        // После линковки отдельные shader objects уже не нужны.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Создаем VAO.
        glGenVertexArrays(1, &s_VertexArray);
        glBindVertexArray(s_VertexArray);

        // Создаем VBO и загружаем вершины в видеопамять.
        glGenBuffers(1, &s_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, s_VertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW);

        // Attribute 0: позиция.
        // stride = 6 float, потому что вершина: x y z r g b.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            nullptr);

        // Attribute 1: цвет.
        // offset = 3 float, потому что цвет начинается после x y z.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            reinterpret_cast<void *>(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Renderer::Shutdown()
    {
        glDeleteProgram(s_ShaderProgram);
        glDeleteBuffers(1, &s_VertexBuffer);
        glDeleteVertexArrays(1, &s_VertexArray);
    }

    void Renderer::BeginFrame()
    {
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::EndFrame()
    {
    }

    void Renderer::RenderScene(
        const Scene &scene,
        const EditorCamera &camera,
        int selectedEntity)
    {
        glUseProgram(s_ShaderProgram);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(s_VertexArray);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

        int mvpLocation = glGetUniformLocation(s_ShaderProgram, "u_MVP");
        int colorLocation = glGetUniformLocation(s_ShaderProgram, "u_Color");
        int useObjectColorLocation = glGetUniformLocation(s_ShaderProgram, "u_UseObjectColor");

        for (int i = 0; i < scene.Entities.size(); i++)
        {
            const auto &entity = scene.Entities[i];

            // ECS-lite логика:
            // если у объекта нет MeshRenderer, Renderer его не рисует.
            if (!entity.HasMeshRenderer || !entity.MeshRenderer.Enabled)
            {
                continue;
            }

            // TransformComponent сам собирает model matrix.
            glm::mat4 model = entity.Transform.GetTransform();

            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(
                mvpLocation,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

            // Если объект выбран — рисуем его оранжевым.
            // Иначе используем цветные грани куба.
            if (i == selectedEntity)
            {
                glUniform1i(useObjectColorLocation, 1);
                glUniform3f(colorLocation, 1.0f, 0.6f, 0.1f);
            }
            else
            {
                // Обычный объект рисуем цветом из MeshRendererComponent.
                glUniform1i(useObjectColorLocation, 1);

                glUniform3f(
                    colorLocation,
                    entity.MeshRenderer.Color.x,
                    entity.MeshRenderer.Color.y,
                    entity.MeshRenderer.Color.z);
            }

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Renderer::RenderScenePicking(
        const Scene &scene,
        const EditorCamera &camera)
    {
        glUseProgram(s_ShaderProgram);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(s_VertexArray);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

        int mvpLocation = glGetUniformLocation(s_ShaderProgram, "u_MVP");
        int colorLocation = glGetUniformLocation(s_ShaderProgram, "u_Color");
        int useObjectColorLocation = glGetUniformLocation(s_ShaderProgram, "u_UseObjectColor");

        // Picking всегда использует объектный цвет.
        glUniform1i(useObjectColorLocation, 1);

        for (int i = 0; i < scene.Entities.size(); i++)
        {
            const auto &entity = scene.Entities[i];

            // Picking нужен только для объектов, которые реально рисуются.
            if (!entity.HasMeshRenderer || !entity.MeshRenderer.Enabled)
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

            // Кодируем индекс Entity в красный канал.
            // 0 означает "ничего", поэтому используем i + 1.
            float idColor = static_cast<float>(i + 1) / 255.0f;
            glUniform3f(colorLocation, idColor, 0.0f, 0.0f);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}