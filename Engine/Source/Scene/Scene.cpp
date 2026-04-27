#include "Scene/Scene.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace RockEngine
{
    void Scene::SaveToFile(const std::string &path)
    {
        std::ofstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to save scene: " << path << std::endl;
            return;
        }

        for (auto &entity : Entities)
        {
            file << "Entity \"" << entity.Name << "\"\n";

            if (entity.Type == EntityType::Mesh)
            {
                file << "Type Mesh\n";
            }
            else if (entity.Type == EntityType::Camera)
            {
                file << "Type Camera\n";
            }
            else if (entity.Type == EntityType::Light)
            {
                file << "Type Light\n";
            }

            file << "Position "
                 << entity.TransformComponent.Position[0] << " "
                 << entity.TransformComponent.Position[1] << " "
                 << entity.TransformComponent.Position[2] << "\n";

            file << "Rotation "
                 << entity.TransformComponent.Rotation[0] << " "
                 << entity.TransformComponent.Rotation[1] << " "
                 << entity.TransformComponent.Rotation[2] << "\n";

            file << "Scale "
                 << entity.TransformComponent.Scale[0] << " "
                 << entity.TransformComponent.Scale[1] << " "
                 << entity.TransformComponent.Scale[2] << "\n";

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

                    if (word == "Type")
                    {
                        std::string typeName;
                        ss >> typeName;

                        if (typeName == "Mesh")
                        {
                            entity.Type = EntityType::Mesh;
                        }
                        else if (typeName == "Camera")
                        {
                            entity.Type = EntityType::Camera;
                        }
                        else if (typeName == "Light")
                        {
                            entity.Type = EntityType::Light;
                        }
                    }
                    else if (word == "Position")
                    {
                        ss >> entity.TransformComponent.Position[0] >> entity.TransformComponent.Position[1] >> entity.TransformComponent.Position[2];
                    }
                    else if (word == "Rotation")
                    {
                        ss >> entity.TransformComponent.Rotation[0] >> entity.TransformComponent.Rotation[1] >> entity.TransformComponent.Rotation[2];
                    }
                    else if (word == "Scale")
                    {
                        ss >> entity.TransformComponent.Scale[0] >> entity.TransformComponent.Scale[1] >> entity.TransformComponent.Scale[2];
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

    void Scene::Clear()
    {
        Entities.clear();
    }

    void Scene::CreateDefaultScene()
    {
        Clear();

        Entity camera;
        camera.Name = "Camera";
        camera.Type = EntityType::Camera;
        camera.TransformComponent.Position[2] = 5.0f;
        Entities.push_back(camera);

        Entity cube;
        cube.Name = "Cube";
        cube.Type = EntityType::Mesh;
        cube.TransformComponent.Rotation[0] = 20.0f;
        cube.TransformComponent.Rotation[1] = 30.0f;
        Entities.push_back(cube);

        Entity light;
        light.Name = "Light";
        light.Type = EntityType::Light;
        light.TransformComponent.Position[0] = 2.0f;
        light.TransformComponent.Position[1] = 2.0f;
        Entities.push_back(light);
    }
}