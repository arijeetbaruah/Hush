#include "engine/input/InputBackend.hpp"
#include "engine/Window.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

namespace eng {
namespace {

struct KeyMap { Control c; int glfwKey; };
const KeyMap kKeys[] = {
    {Control::W, GLFW_KEY_W}, {Control::A, GLFW_KEY_A}, {Control::S, GLFW_KEY_S}, {Control::D, GLFW_KEY_D},
    {Control::Q, GLFW_KEY_Q}, {Control::E, GLFW_KEY_E}, {Control::C, GLFW_KEY_C}, {Control::R, GLFW_KEY_R},
    {Control::F, GLFW_KEY_F}, {Control::Space, GLFW_KEY_SPACE}, {Control::LeftShift, GLFW_KEY_LEFT_SHIFT},
    {Control::LeftControl, GLFW_KEY_LEFT_CONTROL}, {Control::Tab, GLFW_KEY_TAB},
    {Control::Escape, GLFW_KEY_ESCAPE}, {Control::Enter, GLFW_KEY_ENTER},
    {Control::Up, GLFW_KEY_UP}, {Control::Down, GLFW_KEY_DOWN},
    {Control::Left, GLFW_KEY_LEFT}, {Control::Right, GLFW_KEY_RIGHT},
};
const KeyMap kMouseButtons[] = {
    {Control::MouseLeft, GLFW_MOUSE_BUTTON_LEFT}, {Control::MouseRight, GLFW_MOUSE_BUTTON_RIGHT},
    {Control::MouseMiddle, GLFW_MOUSE_BUTTON_MIDDLE},
};
const KeyMap kPadButtons[] = {
    {Control::PadSouth, GLFW_GAMEPAD_BUTTON_A}, {Control::PadEast, GLFW_GAMEPAD_BUTTON_B},
    {Control::PadWest, GLFW_GAMEPAD_BUTTON_X},  {Control::PadNorth, GLFW_GAMEPAD_BUTTON_Y},
    {Control::PadLB, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER}, {Control::PadRB, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER},
    {Control::PadBack, GLFW_GAMEPAD_BUTTON_BACK}, {Control::PadStart, GLFW_GAMEPAD_BUTTON_START},
    {Control::PadLStick, GLFW_GAMEPAD_BUTTON_LEFT_THUMB}, {Control::PadRStick, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB},
    {Control::PadDUp, GLFW_GAMEPAD_BUTTON_DPAD_UP}, {Control::PadDDown, GLFW_GAMEPAD_BUTTON_DPAD_DOWN},
    {Control::PadDLeft, GLFW_GAMEPAD_BUTTON_DPAD_LEFT}, {Control::PadDRight, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT},
};

float deadzone(float v, float dz = 0.15f) {
    float a = std::fabs(v);
    return a < dz ? 0.0f : std::copysign((a - dz) / (1.0f - dz), v);   // rescale so output still reaches 1
}

class GlfwInputBackend : public IInputBackend {
public:
    explicit GlfwInputBackend(GLFWwindow* w) : win_(w) {}

    void poll(RawInput& out) override {
        auto& v = out.value;
        v.fill(0.0f);
        auto at = [&](Control c) -> float& { return v[static_cast<int>(c)]; };

        for (auto& k : kKeys)         at(k.c) = glfwGetKey(win_, k.glfwKey) == GLFW_PRESS ? 1.0f : 0.0f;
        for (auto& b : kMouseButtons) at(b.c) = glfwGetMouseButton(win_, b.glfwKey) == GLFW_PRESS ? 1.0f : 0.0f;

        double x, y;
        glfwGetCursorPos(win_, &x, &y);
        if (first_) { lastX_ = x; lastY_ = y; first_ = false; }
        if (captured_) {
            at(Control::MouseDeltaX) = float(x - lastX_);
            at(Control::MouseDeltaY) = float(lastY_ - y);       // screen Y points down; we want +up
        }
        lastX_ = x; lastY_ = y;

        for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
            GLFWgamepadstate st;
            if (!glfwJoystickIsGamepad(jid) || !glfwGetGamepadState(jid, &st)) continue;
            for (auto& b : kPadButtons) at(b.c) = st.buttons[b.glfwKey] ? 1.0f : 0.0f;
            at(Control::PadLeftX)  =  deadzone(st.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
            at(Control::PadLeftY)  = -deadzone(st.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);    // GLFW: up is -1
            at(Control::PadRightX) =  deadzone(st.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
            at(Control::PadRightY) = -deadzone(st.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
            at(Control::PadLT) = (st.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1.0f) * 0.5f;   // -1..1 -> 0..1
            at(Control::PadRT) = (st.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.0f) * 0.5f;
            break;   // first connected gamepad only (multiplayer would extend this)
        }
    }

    void setCursorCaptured(bool captured) override {
        captured_ = captured;
        glfwSetInputMode(win_, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (captured && glfwRawMouseMotionSupported())
            glfwSetInputMode(win_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        first_ = true;   // avoid a jump when toggling
    }

private:
    GLFWwindow* win_;
    double lastX_ = 0, lastY_ = 0;
    bool first_ = true, captured_ = false;
};

} // namespace

std::unique_ptr<IInputBackend> createPlatformInputBackend(Window& window) {
    return std::make_unique<GlfwInputBackend>(static_cast<GLFWwindow*>(window.nativeHandle()));
}

} // namespace eng