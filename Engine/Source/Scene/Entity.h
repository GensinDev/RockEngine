#pragma once

#include "Scene/Transform.h"

#include <string>

namespace RockEngine
{
    enum class EntityType
    {
        Mesh,
        Camera,
        Light
    };

    struct Entity
    {
        std::string Name;
        EntityType Type = EntityType::Mesh;
        Transform TransformComponent;
    };
}