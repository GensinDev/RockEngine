#pragma once

#include "Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace RockEngine
{
    // =========================
    // TRANSFORM COMPONENT
    // =========================
    // Отвечает за положение объекта в мире:
    // - Position: где объект находится
    // - Rotation: как объект повернут
    // - Scale: размер объекта
    struct TransformComponent
    {
        glm::vec3 Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

        // Собирает итоговую матрицу объекта.
        // Именно эту матрицу Renderer передает в шейдер.
        glm::mat4 GetTransform() const
        {
            glm::mat4 transform = glm::mat4(1.0f);

            transform = glm::translate(transform, Position);

            transform = glm::rotate(
                transform,
                glm::radians(Rotation.x),
                glm::vec3(1.0f, 0.0f, 0.0f));

            transform = glm::rotate(
                transform,
                glm::radians(Rotation.y),
                glm::vec3(0.0f, 1.0f, 0.0f));

            transform = glm::rotate(
                transform,
                glm::radians(Rotation.z),
                glm::vec3(0.0f, 0.0f, 1.0f));

            transform = glm::scale(transform, Scale);

            return transform;
        }
    };

    // =========================
    // MESH RENDERER COMPONENT
    // =========================
    // Если Entity имеет этот компонент,
    // значит Renderer должен его рисовать.
    struct MeshRendererComponent
    {
        bool Enabled = true;

        glm::vec3 Color = {0.2f, 0.7f, 1.0f};

        //  НОВОЕ
        std::shared_ptr<Mesh> MeshPtr;
    };

    // =========================
    // CAMERA COMPONENT
    // =========================
    // Пока это просто маркер.
    // Позже сделаем настоящую игровую камеру.
    struct CameraComponent
    {
        bool Primary = false;
    };

    // =========================
    // LIGHT COMPONENT
    // =========================
    // Пока это тоже маркер.
    // Позже добавим цвет, силу света, радиус и т.д.
    struct LightComponent
    {
        glm::vec3 Color = {1.0f, 1.0f, 1.0f};
        float Intensity = 1.0f;
    };
}