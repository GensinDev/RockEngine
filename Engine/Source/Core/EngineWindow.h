#pragma once // защищает файл от повторного подключения

#include <string>

struct GLFWwindow; // forward declaration

namespace RockEngine
{
    class EngineWindow
    {
    public:
        EngineWindow(const std::string &title, int width, int height); // конструктор. создает окно
        ~EngineWindow();                                               // деструктор. уничтожает окно и освобождает ресурсы

        void OnUpdate(); // Метод, который будет показывать каждый кадр.

        bool ShouldClose() const; // закрыл окно или нет?

        GLFWwindow *GetNativeWindow() const; // Возвращает “родное” окно GLFW. Это нужно ImGui, чтобы подключиться к нашему окну.

    private:
        GLFWwindow *m_Window = nullptr; // указатель на настоящее окно GLFW

        std::string m_Title;
        int m_Width = 0;
        int m_Height = 0;
    };
};
