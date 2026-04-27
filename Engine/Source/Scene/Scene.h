#pragma once

#include "Scene/Entity.h"

#include <string>
#include <vector>

namespace RockEngine
{
    class Scene
    {
    public:
        // Все объекты сцены.
        std::vector<Entity> Entities;

        // Очистить сцену.
        void Clear();

        // Создать стандартную сцену:
        // Camera + Cube + Light.
        void CreateDefaultScene();

        // Сохранить сцену в файл.
        void SaveToFile(const std::string &path);

        // Загрузить сцену из файла.
        void LoadFromFile(const std::string &path);
    };
}