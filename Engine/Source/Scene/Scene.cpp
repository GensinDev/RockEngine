#include "Scene/Scene.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace RockEngine
{
    void Scene::Clear()
    {
        Entities.clear();
    }

    void Scene::CreateDefaultScene()
    {
        Clear();

        // =========================
        // CAMERA
        // =========================
        Entity camera;
        camera.Name = "Camera";
        camera.HasCamera = true;
        camera.Camera.Primary = true;
        camera.Transform.Position = {0.0f, 0.0f, 5.0f};
        Entities.push_back(camera);

        // =========================
        // CUBE
        // =========================
        Entity cube;
        cube.Name = "Cube";
        cube.HasMeshRenderer = true;
        cube.Transform.Rotation = {20.0f, 30.0f, 0.0f};
        Entities.push_back(cube);

        // =========================
        // LIGHT
        // =========================
        Entity light;
        light.Name = "Light";
        light.HasLight = true;
        light.Transform.Position = {2.0f, 2.0f, 0.0f};
        Entities.push_back(light);
    }

    void Scene::SaveToFile(const std::string &path)
    {
        std::ofstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to save scene: " << path << std::endl;
            return;
        }

        for (const auto &entity : Entities)
        {
            file << "Entity \"" << entity.Name << "\"\n";

            // Сохраняем компоненты.
            file << "HasMeshRenderer " << entity.HasMeshRenderer << "\n";
            file << "HasCamera " << entity.HasCamera << "\n";
            file << "HasLight " << entity.HasLight << "\n";
            file << "MeshRendererEnabled " << entity.MeshRenderer.Enabled << "\n";
            file << "MeshRendererColor "
                 << entity.MeshRenderer.Color.x << " "
                 << entity.MeshRenderer.Color.y << " "
                 << entity.MeshRenderer.Color.z << "\n";
            // Если у объекта есть CameraComponent,
            // сохраняем является ли она основной камерой.
            file << "CameraPrimary " << entity.Camera.Primary << "\n";
            // Если у объекта есть LightComponent,
            // сохраняем цвет и интенсивность света.
            file << "LightColor "
                 << entity.Light.Color.x << " "
                 << entity.Light.Color.y << " "
                 << entity.Light.Color.z << "\n";
            file << "LightIntensity "
                 << entity.Light.Intensity << "\n";

            file << "Position "
                 << entity.Transform.Position.x << " "
                 << entity.Transform.Position.y << " "
                 << entity.Transform.Position.z << "\n";

            file << "Rotation "
                 << entity.Transform.Rotation.x << " "
                 << entity.Transform.Rotation.y << " "
                 << entity.Transform.Rotation.z << "\n";

            file << "Scale "
                 << entity.Transform.Scale.x << " "
                 << entity.Transform.Scale.y << " "
                 << entity.Transform.Scale.z << "\n";

            file << "EndEntity\n";
        }
    }

    void Scene::LoadFromFile(const std::string &path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to load scene: " << path << std::endl;
            return;
        }

        Entities.clear();

        std::string line;

        while (std::getline(file, line))
        {
            if (line.rfind("Entity ", 0) == 0)
            {
                Entity entity;

                size_t firstQuote = line.find('"');
                size_t lastQuote = line.rfind('"');

                if (firstQuote != std::string::npos &&
                    lastQuote != std::string::npos &&
                    lastQuote > firstQuote)
                {
                    entity.Name = line.substr(
                        firstQuote + 1,
                        lastQuote - firstQuote - 1);
                }

                while (std::getline(file, line))
                {
                    std::stringstream ss(line);
                    std::string word;
                    ss >> word;

                    if (word == "HasMeshRenderer")
                    {
                        ss >> entity.HasMeshRenderer;
                    }
                    else if (word == "HasCamera")
                    {
                        ss >> entity.HasCamera;
                    }
                    else if (word == "HasLight")
                    {
                        ss >> entity.HasLight;
                    }
                    else if (word == "MeshRendererEnabled")
                    {
                        ss >> entity.MeshRenderer.Enabled;
                    }
                    else if (word == "MeshRendererColor")
                    {
                        ss >> entity.MeshRenderer.Color.x >> entity.MeshRenderer.Color.y >> entity.MeshRenderer.Color.z;
                    }
                    else if (word == "CameraPrimary")
                    {
                        // Загружаем настройку основной камеры.
                        ss >> entity.Camera.Primary;
                    }
                    else if (word == "LightColor")
                    {
                        // Загружаем цвет света.
                        ss >> entity.Light.Color.x >> entity.Light.Color.y >> entity.Light.Color.z;
                    }
                    else if (word == "LightIntensity")
                    {
                        // Загружаем силу света.
                        ss >> entity.Light.Intensity;
                    }
                    else if (word == "Position")
                    {
                        ss >> entity.Transform.Position.x >> entity.Transform.Position.y >> entity.Transform.Position.z;
                    }
                    else if (word == "Rotation")
                    {
                        ss >> entity.Transform.Rotation.x >> entity.Transform.Rotation.y >> entity.Transform.Rotation.z;
                    }
                    else if (word == "Scale")
                    {
                        ss >> entity.Transform.Scale.x >> entity.Transform.Scale.y >> entity.Transform.Scale.z;
                    }
                    else if (word == "EndEntity")
                    {
                        break;
                    }
                }

                Entities.push_back(entity);
            }
        }
    }
}