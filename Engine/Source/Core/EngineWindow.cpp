#include "Core/EngineWindow.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace RockEngine
{
    EngineWindow::EngineWindow(const std::string &title, int width, int height) // конструктор класса Window
        : m_Title(title), m_Width(width), m_Height(height)                      // список инициализации
    {
        if (!glfwInit()) // инициализирует GLFW
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return;
        }

        m_Window = glfwCreateWindow( // создаем окно
            m_Width,
            m_Height,
            m_Title.c_str(),
            nullptr,
            nullptr);

        if (!m_Window)
        {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate(); // завершение работы GLFW и освобождение ресурсов
            return;
        }

        glfwMakeContextCurrent(m_Window); // Все будущие команды рендера будут относиться к этому окну.

        glewExperimental = GL_TRUE;

        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            return;
        }
    }

    EngineWindow::~EngineWindow() // деструктор
    {
        if (m_Window) // если окно есть уничтожаем его
        {
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    void EngineWindow::OnUpdate()
    {
        glfwPollEvents();          // обработчик событий
        glfwSwapBuffers(m_Window); // показывает готовый кадр
    }

    bool EngineWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    GLFWwindow *EngineWindow::GetNativeWindow() const
    {
        return m_Window;
    }
}