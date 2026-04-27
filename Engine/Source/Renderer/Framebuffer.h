#pragma once

#include <cstdint>

namespace RockEngine
{
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height);
        ~Framebuffer();

        void Bind();
        void Unbind();

        void Resize(uint32_t width, uint32_t height);

        uint32_t GetColorAttachment() const { return m_ColorAttachment; } // передаем текстуру вImGui, чтобы показает ее в окне Viewport

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        void Invalidate();

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0; // текстура с картинкой сцены
        uint32_t m_DepthAttachment = 0;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };
}