#include "engine/input/ActionMap.hpp"
#include <cstdlib>
#include <sstream>

namespace eng {

void ActionMap::define(ActionId id, const std::string& name, bool clamp, bool rate) {
    if (id < 0) return;
    if (static_cast<size_t>(id) >= defs_.size()) defs_.resize(id + 1);
    defs_[id].name = name;
    defs_[id].clamp = clamp;
    defs_[id].rate = rate;
}

void ActionMap::bind(ActionId id, Control c, float scale) {
    if (id >= 0 && static_cast<size_t>(id) < defs_.size()) defs_[id].bindings.push_back({c, scale});
}

void ActionMap::clearBindings(ActionId id) {
    if (id >= 0 && static_cast<size_t>(id) < defs_.size()) defs_[id].bindings.clear();
}

void ActionMap::rebind(ActionId id, size_t slot, Control c) {
    if (id < 0 || static_cast<size_t>(id) >= defs_.size()) return;
    auto& b = defs_[id].bindings;
    if (slot < b.size()) b[slot].control = c;
}

const ActionDef* ActionMap::find(ActionId id) const {
    if (id < 0 || static_cast<size_t>(id) >= defs_.size() || defs_[id].name.empty()) return nullptr;
    return &defs_[id];
}

std::string ActionMap::serialize() const {
    std::ostringstream out;
    for (const auto& d : defs_) {
        if (d.name.empty()) continue;
        out << d.name << ':';
        for (const auto& b : d.bindings) out << ' ' << controlName(b.control) << '*' << b.scale;
        out << '\n';
    }
    return out.str();
}

void ActionMap::deserialize(const std::string& text) {
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string name = line.substr(0, colon);

        ActionDef* def = nullptr;
        for (auto& d : defs_) if (d.name == name) def = &d;
        if (!def) continue;                       // unknown action: ignore (old save files stay safe)

        std::vector<Binding> list;
        std::istringstream toks(line.substr(colon + 1));
        std::string t;
        while (toks >> t) {
            auto star = t.find('*');
            if (star == std::string::npos) continue;
            Control c;
            if (!controlFromName(t.substr(0, star), c)) continue;   // unknown control: skip it
            list.push_back({c, std::strtof(t.c_str() + star + 1, nullptr)});
        }
        def->bindings = std::move(list);
    }
}

} // namespace eng