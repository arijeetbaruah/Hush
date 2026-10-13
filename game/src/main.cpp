#include "engine/Window.hpp"
#include "engine/input/Input.hpp"
#include "engine/Camera.hpp"
#include <GL/glew.h>   // temporary until the Renderer (Step 3)
#include <algorithm>
#include <cstdio>

enum Act : eng::ActionId { MoveX, MoveY, Fly, Sprint, LookX, LookY, Quit, ToggleCursor, Rebind };

static void setupInput(eng::Input& input) {
    using C = eng::Control;
    auto& a = input.actions();

    a.define(MoveX, "MoveX", /*clamp*/ true);
    a.define(MoveY, "MoveY", true);
    a.define(Fly, "Fly", true);
    a.define(Sprint, "Sprint", true);
    a.define(LookX, "LookX", /*clamp*/ false, /*rate*/ true);
    a.define(LookY, "LookY", false, true);
    a.define(Quit, "Quit", true);
    a.define(ToggleCursor, "ToggleCursor", true);
    a.define(Rebind, "Rebind", true);

    a.bind(MoveY, C::W, 1);  a.bind(MoveY, C::S, -1);  a.bind(MoveY, C::PadLeftY, 1);
    a.bind(MoveX, C::D, 1);  a.bind(MoveX, C::A, -1);  a.bind(MoveX, C::PadLeftX, 1);
    a.bind(Fly, C::Space, 1); a.bind(Fly, C::LeftControl, -1);
    a.bind(Fly, C::PadRT, 1); a.bind(Fly, C::PadLT, -1);
    a.bind(Sprint, C::LeftShift, 1); a.bind(Sprint, C::PadLStick, 1);

    // Look is in radians. Mouse: radians per pixel. Stick: radians per second. Negative X = right turns right.
    a.bind(LookX, C::MouseDeltaX, -0.0022f); a.bind(LookX, C::PadRightX, -2.5f);
    a.bind(LookY, C::MouseDeltaY,  0.0022f); a.bind(LookY, C::PadRightY,  2.0f);

    a.bind(Quit, C::Escape, 1);
    a.bind(ToggleCursor, C::Tab, 1);
    a.bind(Rebind, C::R, 1);
}

int main() {
    try {
        eng::Window window(1280, 720, "Hush");
        eng::Input  input(window);
        eng::Camera camera;
        camera.position = {0.0f, 1.7f, 5.0f};

        setupInput(input);
        std::printf("--- default bindings ---\n%s------------------------\n",
                    input.actions().serialize().c_str());

        double last = window.time();
        float titleTimer = 0.0f;
        const char* status = "";

        while (!window.shouldClose()) {
            window.pollEvents();
            double now = window.time();
            float dt = std::min(0.05f, float(now - last));
            last = now;
            input.update(dt);

            if (input.pressed(Quit)) window.close();
            if (input.pressed(ToggleCursor)) input.setCursorCaptured(!input.cursorCaptured());

            // Rebinding demo: press R, then press any key/button: it becomes Sprint's first binding.
            if (input.pressed(Rebind)) { input.beginCapture(); status = "PRESS A KEY FOR SPRINT..."; }
            eng::Control picked;
            if (input.takeCaptured(picked)) {
                input.actions().rebind(Sprint, 0, picked);
                status = "Sprint rebound";
                std::printf("--- bindings now ---\n%s--------------------\n", input.actions().serialize().c_str());
            }

            if (input.cursorCaptured()) camera.rotate(input.value(LookX), input.value(LookY));

            glm::vec3 move = camera.flatForward() * input.value(MoveY) + camera.right() * input.value(MoveX);
            move.y += input.value(Fly);
            if (glm::length(move) > 1.0f) move = glm::normalize(move);
            camera.position += move * (input.down(Sprint) ? 10.0f : 5.0f) * dt;

            titleTimer -= dt;
            if (titleTimer <= 0.0f) {
                titleTimer = 0.1f;
                static const char* dev[] = {"Keyboard", "Mouse", "Gamepad"};
                char buf[200];
                std::snprintf(buf, sizeof buf, "Hush | pos (%.1f, %.1f, %.1f) | yaw %.0f pitch %.0f | %s | %s",
                              camera.position.x, camera.position.y, camera.position.z,
                              glm::degrees(camera.yaw), glm::degrees(camera.pitch),
                              dev[static_cast<int>(input.lastDevice())], status);
                window.setTitle(buf);
            }

            glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            window.swapBuffers();
        }
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return 1;
    }
    return 0;
}
