#pragma once

#include "Scene/Entity.h"

#include <vector>
#include <string>

namespace RockEngine
{
    class Scene
    {
    public:
        std::vector<Entity> Entities;

        void Clear();
        void CreateDefaultScene();

        void SaveToFile(const std::string &path);
        void LoadFromFile(const std::string &path);
    };
}