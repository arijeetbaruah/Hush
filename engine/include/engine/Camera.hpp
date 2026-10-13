#pragma once
#include <glm/glm.hpp>

namespace eng {

    // First-person camera. Convention: yaw = 0 looks down -Z, +Y is up, right-handed.
    // We store yaw/pitch (not a matrix) so game code can say "turn left" or
    // "get the horizontal forward direction" without matrix math.
    class Camera {
    public:
        glm::vec3 position{0.0f, 1.7f, 0.0f};
        float yaw   = 0.0f;                     // radians, rotation around +Y
        float pitch = 0.0f;                     // radians, clamped to just under +-90 degrees
        float fov   = glm::radians(75.0f);      // vertical field of view
        float nearZ = 0.05f;
        float farZ  = 200.0f;

        void look(float dxPixels, float dyPixels, float sensitivity = 0.0022f);

        glm::vec3 forward() const;       // where the camera looks (includes pitch)
        glm::vec3 flatForward() const;   // same, projected flat: use this for walking
        glm::vec3 right() const;

        glm::mat4 view() const;
        glm::mat4 projection(float aspect) const;
        
        void rotate(float yawDelta, float pitchDelta);
    };

}