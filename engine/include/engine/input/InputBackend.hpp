#pragma once
#include "engine/input/Controls.hpp"
#include <array>
#include <memory>

namespace eng {

    class Window;

    // One float per control: buttons are 0 or 1, axes are -1..1 (triggers 0..1),
    // mouse deltas are pixels this frame. The backend normalizes sign conventions
    // and applies stick deadzones, so nothing above this layer is platform-specific.
    struct RawInput {
        std::array<float, kControlCount> value{};
    };

    class IInputBackend {
    public:
        virtual ~IInputBackend() = default;
        virtual void poll(RawInput& out) = 0;               // call once per frame
        virtual void setCursorCaptured(bool captured) = 0;  // no-op on consoles
    };

    // Defined once per platform, in src/platform/<name>/. CMake chooses which file is compiled.
    std::unique_ptr<IInputBackend> createPlatformInputBackend(Window& window);

} // namespace eng