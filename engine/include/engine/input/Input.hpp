#pragma once
#include "engine/input/ActionMap.hpp"
#include "engine/input/InputBackend.hpp"
#include <memory>
#include <vector>

namespace eng {

    class Window;

    class Input {
    public:
        explicit Input(Window& window);                          // uses the platform backend
        explicit Input(std::unique_ptr<IInputBackend> backend);  // custom / test backend

        ActionMap& actions() { return map_; }

        void update(float dt);          // once per frame, after window.pollEvents()

        float value(ActionId id) const;
        bool  down(ActionId id) const;       // held
        bool  pressed(ActionId id) const;    // went down this frame
        bool  released(ActionId id) const;   // went up this frame

        float  raw(Control c) const { return raw_.value[static_cast<int>(c)]; }
        Device lastDevice() const { return lastDevice_; }   // drives button prompts: keyboard vs pad icons

        void setCursorCaptured(bool captured);
        bool cursorCaptured() const { return captured_; }

        // Rebinding UI support: beginCapture(), then poll takeCaptured() each frame.
        void beginCapture() { capturing_ = true; hasCaptured_ = false; }
        bool capturing() const { return capturing_; }
        bool takeCaptured(Control& out);

    private:
        float at(const std::vector<float>& v, ActionId id) const;

        std::unique_ptr<IInputBackend> backend_;
        ActionMap map_;
        RawInput raw_, prevRaw_;
        std::vector<float> curr_, prev_;
        Device lastDevice_ = Device::Keyboard;
        bool captured_ = false;
        bool capturing_ = false, hasCaptured_ = false;
        Control capturedControl_ = Control::Count;
    };

} // namespace eng