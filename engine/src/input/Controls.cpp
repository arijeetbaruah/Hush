#include "engine/input/Controls.hpp"

namespace eng {

    static const char* kNames[] = {
#define X(name, dev, delta) #name,
        ENG_CONTROL_LIST(X)
    #undef X
    };
    static const Device kDevices[] = {
#define X(name, dev, delta) Device::dev,
        ENG_CONTROL_LIST(X)
    #undef X
    };
    static const bool kDelta[] = {
#define X(name, dev, delta) delta,
        ENG_CONTROL_LIST(X)
    #undef X
    };

    const char* controlName(Control c) { return kNames[static_cast<int>(c)]; }
    Device controlDevice(Control c)    { return kDevices[static_cast<int>(c)]; }
    bool controlIsDelta(Control c)     { return kDelta[static_cast<int>(c)]; }

    bool controlFromName(std::string_view name, Control& out) {
        for (int i = 0; i < kControlCount; ++i)
            if (name == kNames[i]) { out = static_cast<Control>(i); return true; }
        return false;
    }

} // namespace eng