#include "Renderer/Mesh.h"

#include <GL/glew.h>

namespace RockEngine
{
    Mesh::Mesh(const std::vector<Vertex> &vertices)
    {
        m_VertexCount = (uint32_t)vertices.size();

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void *)offsetof(Vertex, Position));

        // Color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void *)offsetof(Vertex, Color));

        glBindVertexArray(0);
    }

    void Mesh::Bind() const
    {
        glBindVertexArray(m_VAO);
    }

    void Mesh::Draw() const
    {
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    }
}