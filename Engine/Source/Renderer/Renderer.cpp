#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"

#include "Scene/Scene.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace RockEngine
{
    // OpenGL ID объекта, который хранит настройки вершин:
    // какие данные есть у вершины, где позиция, цвет и т.д.
    unsigned int Renderer::s_VertexArray = 0;

    // OpenGL ID буфера, где лежат сами вершины куба.
    unsigned int Renderer::s_VertexBuffer = 0;

    // OpenGL ID скомпилированной shader program.
    // Shader program = vertex shader + fragment shader.
    unsigned int Renderer::s_ShaderProgram = 0;

    void Renderer::Init()
    {
        // Depth Test нужен, чтобы ближние полигоны перекрывали дальние.
        // Без этого грани куба могут рисоваться в неправильном порядке.
        glEnable(GL_DEPTH_TEST);

        // Back-face culling отключает отрисовку задних граней.
        // Это ускоряет рендер и делает куб чище визуально.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Формат каждой вершины:
        //
        // x, y, z, r, g, b
        //
        // x y z — позиция вершины
        // r g b — цвет вершины
        //
        // Куб состоит из 6 граней.
        // Каждая грань = 2 треугольника.
        // Каждый треугольник = 3 вершины.
        //
        // 6 граней * 2 треугольника * 3 вершины = 36 вершин.
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

        // =========================
        // SHADERS
        // =========================

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

        // Компилируем vertex shader.
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "Vertex Shader Error:\n"
                      << infoLog << std::endl;
        }

        // Компилируем fragment shader.
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

        // Создаем shader program.
        s_ShaderProgram = glCreateProgram();

        // Прикрепляем оба шейдера к программе.
        glAttachShader(s_ShaderProgram, vertexShader);
        glAttachShader(s_ShaderProgram, fragmentShader);

        // Линкуем программу.
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

        // =========================
        // VAO / VBO
        // =========================

        glGenVertexArrays(1, &s_VertexArray);
        glBindVertexArray(s_VertexArray);

        glGenBuffers(1, &s_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, s_VertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW);

        // Attribute 0: Position.
        //
        // Вершина имеет формат:
        // x y z r g b
        //
        // Позиция начинается с 0-го float.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            nullptr);

        // Attribute 1: Color.
        //
        // Цвет начинается после первых трех float:
        // x y z | r g b
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

            if (entity.Type != EntityType::Mesh)
            {
                continue;
            }

            const auto &t = entity.TransformComponent;

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(
                model,
                glm::vec3(t.Position[0], t.Position[1], t.Position[2]));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[0]),
                glm::vec3(1.0f, 0.0f, 0.0f));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[1]),
                glm::vec3(0.0f, 1.0f, 0.0f));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[2]),
                glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(
                model,
                glm::vec3(t.Scale[0], t.Scale[1], t.Scale[2]));

            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(
                mvpLocation,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

            // Если объект выбран — рисуем его оранжевым.
            // Если не выбран — используем цвета вершин куба.
            if (i == selectedEntity)
            {
                glUniform1i(useObjectColorLocation, 1);
                glUniform3f(colorLocation, 1.0f, 0.6f, 0.1f);
            }
            else
            {
                glUniform1i(useObjectColorLocation, 0);
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

            if (entity.Type != EntityType::Mesh)
            {
                continue;
            }

            const auto &t = entity.TransformComponent;

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(
                model,
                glm::vec3(t.Position[0], t.Position[1], t.Position[2]));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[0]),
                glm::vec3(1.0f, 0.0f, 0.0f));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[1]),
                glm::vec3(0.0f, 1.0f, 0.0f));

            model = glm::rotate(
                model,
                glm::radians(t.Rotation[2]),
                glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(
                model,
                glm::vec3(t.Scale[0], t.Scale[1], t.Scale[2]));

            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(
                mvpLocation,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

            // Кодируем индекс entity в красный канал.
            // i + 1 нужно, потому что 0 означает "ничего не выбрано".
            float idColor = static_cast<float>(i + 1) / 255.0f;

            glUniform3f(colorLocation, idColor, 0.0f, 0.0f);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}