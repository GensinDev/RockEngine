#pragma once

#include "Scene/Components.h"

#include <string>

namespace RockEngine
{
    // =========================
    // ENTITY
    // =========================
    // Entity — это объект сцены.
    //
    // Сейчас это ECS-lite:
    // у объекта есть Transform всегда,
    // а остальные компоненты включаются через Has...
    //
    // Позже мы можем заменить это на настоящий ECS через EnTT.
    struct Entity
    {
        std::string Name = "Entity";

        // Transform есть у каждого объекта.
        TransformComponent Transform;

        // Компоненты-маркеры.
        bool HasMeshRenderer = false;
        bool HasCamera = false;
        bool HasLight = false;

        MeshRendererComponent MeshRenderer;
        CameraComponent Camera;
        LightComponent Light;
    };
}