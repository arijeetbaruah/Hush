#pragma once
#include <memory>
#include <string>

namespace eng {

    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose() const;
        void close();
        void pollEvents();
        void swapBuffers();
        void setTitle(const std::string& title);
        void framebufferSize(int& w, int& h) const;
        double time() const;               // seconds

        // Platform-specific handle. Only code under src/platform/ may cast this.
        void* nativeHandle() const;

    private:
        struct Impl;                       // defined per platform
        std::unique_ptr<Impl> impl_;
    };

} // namespace eng