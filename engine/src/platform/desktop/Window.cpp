#include "engine/Window.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <stdexcept>

namespace eng {

    struct Window::Impl {
        GLFWwindow* win = nullptr;
    };

    Window::Window(int width, int height, const std::string& title) : impl_(std::make_unique<Impl>()) {
        if (!glfwInit()) throw std::runtime_error("glfwInit failed");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        impl_->win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!impl_->win) { glfwTerminate(); throw std::runtime_error("window creation failed"); }

        glfwMakeContextCurrent(impl_->win);
        glfwSwapInterval(1);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) throw std::runtime_error("glewInit failed");
        std::printf("OpenGL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    }

    Window::~Window() {
        if (impl_ && impl_->win) glfwDestroyWindow(impl_->win);
        glfwTerminate();
    }

    bool Window::shouldClose() const { return glfwWindowShouldClose(impl_->win); }
    void Window::close() { glfwSetWindowShouldClose(impl_->win, GLFW_TRUE); }
    void Window::pollEvents() { glfwPollEvents(); }
    void Window::swapBuffers() { glfwSwapBuffers(impl_->win); }
    void Window::setTitle(const std::string& t) { glfwSetWindowTitle(impl_->win, t.c_str()); }
    void Window::framebufferSize(int& w, int& h) const { glfwGetFramebufferSize(impl_->win, &w, &h); }
    double Window::time() const { return glfwGetTime(); }
    void* Window::nativeHandle() const { return impl_->win; }

} // namespace eng