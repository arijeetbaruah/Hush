#pragma once
#include <cstdint>
#include <string_view>

namespace eng {

enum class Device : uint8_t { Keyboard, Mouse, Gamepad };

// X(name, device, isDelta)
// isDelta = value is "movement this frame" (mouse). Everything else is a held state or a rate.
// Axis conventions: stick X +right, stick Y +up, MouseDeltaX +right, MouseDeltaY +up, triggers 0..1.
#define ENG_CONTROL_LIST(X)                                                        \
    X(W, Keyboard, false) X(A, Keyboard, false) X(S, Keyboard, false)              \
    X(D, Keyboard, false) X(Q, Keyboard, false) X(E, Keyboard, false)              \
    X(C, Keyboard, false) X(R, Keyboard, false) X(F, Keyboard, false)              \
    X(Space, Keyboard, false) X(LeftShift, Keyboard, false)                        \
    X(LeftControl, Keyboard, false) X(Tab, Keyboard, false)                        \
    X(Escape, Keyboard, false) X(Enter, Keyboard, false)                           \
    X(Up, Keyboard, false) X(Down, Keyboard, false)                                \
    X(Left, Keyboard, false) X(Right, Keyboard, false)                             \
    X(MouseLeft, Mouse, false) X(MouseRight, Mouse, false)                         \
    X(MouseMiddle, Mouse, false)                                                   \
    X(MouseDeltaX, Mouse, true) X(MouseDeltaY, Mouse, true)                        \
    X(PadSouth, Gamepad, false) X(PadEast, Gamepad, false)                         \
    X(PadWest, Gamepad, false) X(PadNorth, Gamepad, false)                         \
    X(PadLB, Gamepad, false) X(PadRB, Gamepad, false)                              \
    X(PadBack, Gamepad, false) X(PadStart, Gamepad, false)                         \
    X(PadLStick, Gamepad, false) X(PadRStick, Gamepad, false)                      \
    X(PadDUp, Gamepad, false) X(PadDDown, Gamepad, false)                          \
    X(PadDLeft, Gamepad, false) X(PadDRight, Gamepad, false)                       \
    X(PadLeftX, Gamepad, false) X(PadLeftY, Gamepad, false)                        \
    X(PadRightX, Gamepad, false) X(PadRightY, Gamepad, false)                      \
    X(PadLT, Gamepad, false) X(PadRT, Gamepad, false)

enum class Control : uint16_t {
#define X(name, dev, delta) name,
    ENG_CONTROL_LIST(X)
#undef X
    Count
};

constexpr int kControlCount = static_cast<int>(Control::Count);

const char* controlName(Control c);                          // for save files and UI
bool        controlFromName(std::string_view name, Control& out);
Device      controlDevice(Control c);
bool        controlIsDelta(Control c);

} // namespace eng