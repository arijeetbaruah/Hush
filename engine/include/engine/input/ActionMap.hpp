#pragma once
#include "engine/input/Controls.hpp"
#include <string>
#include <vector>

namespace eng {

    using ActionId = int;   // the game defines its own enum and casts/uses it directly

    struct Binding {
        Control control;
        float scale = 1.0f;     // sign flips direction, magnitude is sensitivity. Also how "invert Y" works.
    };

    struct ActionDef {
        std::string name;
        bool clamp = false;               // clamp the summed value to [-1, 1] (movement, buttons)
        bool rate = false;                // non-delta controls are per-second: multiplied by dt (look)
        std::vector<Binding> bindings;
    };

    class ActionMap {
    public:
        void define(ActionId id, const std::string& name, bool clamp = false, bool rate = false);
        void bind(ActionId id, Control c, float scale = 1.0f);
        void clearBindings(ActionId id);
        void rebind(ActionId id, size_t slot, Control c);   // swap the control, keep the scale

        const ActionDef* find(ActionId id) const;
        size_t size() const { return defs_.size(); }

        // Text format, one line per action:  MoveY: W*1 S*-1 PadLeftY*1
        // Reading/writing the file is the platform's job (consoles use save-data APIs).
        std::string serialize() const;
        void deserialize(const std::string& text);

    private:
        std::vector<ActionDef> defs_;   // indexed by ActionId
    };

} // namespace eng