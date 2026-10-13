#include "engine/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace eng {

    void Camera::look(float dx, float dy, float s) {
        yaw -= dx * s;                                        // mouse right => turn right => yaw decreases
        pitch = std::clamp(pitch - dy * s, -1.5f, 1.5f);      // mouse down => look down
    }

    glm::vec3 Camera::forward() const {
        return { -std::sin(yaw) * std::cos(pitch), std::sin(pitch), -std::cos(yaw) * std::cos(pitch) };
    }
    glm::vec3 Camera::flatForward() const { return { -std::sin(yaw), 0.0f, -std::cos(yaw) }; }
    glm::vec3 Camera::right() const       { return {  std::cos(yaw), 0.0f, -std::sin(yaw) }; }

    glm::mat4 Camera::view() const {
        return glm::lookAt(position, position + forward(), glm::vec3(0, 1, 0));
    }
    glm::mat4 Camera::projection(float aspect) const {
        return glm::perspective(fov, aspect, nearZ, farZ);
    }
    
    void Camera::rotate(float yawDelta, float pitchDelta) {
        yaw += yawDelta;
        pitch = std::clamp(pitch + pitchDelta, -1.5f, 1.5f);
    }

} // namespace eng