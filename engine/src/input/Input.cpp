#include "engine/input/Input.hpp"
#include "engine/Window.hpp"
#include <algorithm>
#include <cmath>

namespace eng {

static constexpr float kPressThreshold = 0.5f;

Input::Input(Window& window) : backend_(createPlatformInputBackend(window)) { setCursorCaptured(true); }
Input::Input(std::unique_ptr<IInputBackend> backend) : backend_(std::move(backend)) {}

void Input::update(float dt) {
    prevRaw_ = raw_;
    backend_->poll(raw_);

    for (int c = 0; c < kControlCount; ++c) {
        if (std::fabs(raw_.value[c]) > kPressThreshold) lastDevice_ = controlDevice(static_cast<Control>(c));
        if (capturing_ && !hasCaptured_ && !controlIsDelta(static_cast<Control>(c)) &&
            raw_.value[c] > kPressThreshold && prevRaw_.value[c] <= kPressThreshold) {
            capturedControl_ = static_cast<Control>(c);
            hasCaptured_ = true;
            capturing_ = false;
        }
    }

    const size_t n = map_.size();
    prev_ = curr_;
    prev_.resize(n, 0.0f);
    curr_.assign(n, 0.0f);
    for (size_t id = 0; id < n; ++id) {
        const ActionDef* def = map_.find(static_cast<ActionId>(id));
        if (!def) continue;
        float sum = 0.0f;
        for (const Binding& b : def->bindings) {
            float v = raw_.value[static_cast<int>(b.control)];
            if (def->rate && !controlIsDelta(b.control)) v *= dt;   // sticks/keys become "per second"
            sum += v * b.scale;
        }
        if (def->clamp) sum = std::clamp(sum, -1.0f, 1.0f);
        curr_[id] = sum;
    }
}

float Input::at(const std::vector<float>& v, ActionId id) const {
    return (id >= 0 && static_cast<size_t>(id) < v.size()) ? v[id] : 0.0f;
}
float Input::value(ActionId id) const    { return at(curr_, id); }
bool  Input::down(ActionId id) const     { return at(curr_, id) > kPressThreshold; }
bool  Input::pressed(ActionId id) const  { return at(curr_, id) > kPressThreshold && at(prev_, id) <= kPressThreshold; }
bool  Input::released(ActionId id) const { return at(curr_, id) <= kPressThreshold && at(prev_, id) > kPressThreshold; }

void Input::setCursorCaptured(bool captured) {
    captured_ = captured;
    backend_->setCursorCaptured(captured);
}

bool Input::takeCaptured(Control& out) {
    if (!hasCaptured_) return false;
    out = capturedControl_;
    hasCaptured_ = false;
    return true;
}

} // namespace eng