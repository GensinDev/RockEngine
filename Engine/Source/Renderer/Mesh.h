#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace RockEngine
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex> &vertices);

        void Bind() const;
        void Draw() const;

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;

        uint32_t m_VertexCount = 0;
    };
}