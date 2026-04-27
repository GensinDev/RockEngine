#pragma once

namespace RockEngine
{
    struct Transform
    {
        float Position[3] = {0.0f, 0.0f, 0.0f};
        float Rotation[3] = {0.0f, 0.0f, 0.0f};
        float Scale[3] = {1.0f, 1.0f, 1.0f};
    };
}