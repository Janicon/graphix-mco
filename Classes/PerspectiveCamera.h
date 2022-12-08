#pragma once

class PerspectiveCamera : public Camera {
private:
    glm::mat4 base_projection_tpp = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        100.f
    );
    glm::mat4 base_projection_fpp = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        200.f
    );
    float pitch = -25.f;
    float yaw = 90.f;

public:
    PerspectiveCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp, bool istpp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        if (istpp)
            projection = base_projection_tpp;
        else
            projection = base_projection_fpp;
    }

    // Revolve the camera based on a given pitch and yaw
    void revolve(double yawDelta, double pitchDelta, glm::vec3 playerpos) {
        // Distance from focal point
        static float distance = 1.7;

        yaw += yawDelta;
        pitch += pitchDelta;

        // Limit pitch to straight up and down
        if (pitch > 89.f)
            pitch = 89.f;
        if (pitch < -89.f)
            pitch = -89.f;

        position[0] = -cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;
        position[1] = sin(glm::radians(pitch) * -1) * distance;
        position[2] = -sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;

        position += playerpos;

        target = playerpos;
    }

    void adjustCameraTpp(glm::vec3 playerpos, glm::vec3 playerrot) {
        static float distance = 1.5;

        position[0] = -sin(glm::radians(playerrot.x)) * distance + playerpos.x;
        position[1] = playerpos.y + 0.75f;
        position[2] = -cos(glm::radians(playerrot.x)) * distance + playerpos.z;

        target = playerpos;
    }

    void adjustCameraFpp(glm::vec3 playerpos, glm::vec3 playerrot) {
        float newRot = playerrot.x - 180.f;

        float offsetX = -sin(glm::radians(newRot));
        float offsetY = -cos(glm::radians(newRot));

        position = playerpos;

        target = position;
        target[0] += offsetX;
        target[2] += offsetY;
    }

    void setYawPitch(glm::vec3 playerrot) {
        pitch = -25.f;
        yaw = 90.f - playerrot.x;
    }

};