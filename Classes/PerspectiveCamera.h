#pragma once

class PerspectiveCamera : public Camera {
private:
    const glm::mat4 base_projection = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        100.f
    );
    float pitch = 0.f;
    float yaw = 90.f;

public:
    PerspectiveCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        projection = base_projection;
    }

    // Revolve the camera based on a given pitch and yaw
    void revolve(double yawDelta, double pitchDelta) {
        // Distance from focal point
        static float distance = 1.5;

        yaw += yawDelta;
        pitch += pitchDelta;

        // Limit pitch to straight up and down
        if (pitch > 89.f)
            pitch = 89.f;
        if (pitch < -89.f)
            pitch = -89.f;

        position[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;
        position[1] = sin(glm::radians(pitch) * -1) * distance;
        position[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;
    }
};