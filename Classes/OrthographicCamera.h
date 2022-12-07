#pragma once

class OrthographicCamera : public Camera {
private:
    const glm::mat4 base_projection = glm::ortho(
        -2.0f, 2.0f,
        -2.0f, 2.0f,
        -0.1f, 100.0f
    );
    float angle = 90;

public:
    OrthographicCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        projection = base_projection;
    }

    void panCamera(glm::vec3 position) {
        this->position += position;
        this->target += position;
    }

    void dragCamera(float angle) {
        this->angle += angle;

        // Limit pitch to straight up and down
        if (this->angle > 90.f) {
            this->angle = 90.f;
            return;
        }
        if (this->angle < 15) {
            this->angle = 15.f;
            return;
        }

        position[1] = target[1] + sin(glm::radians(this->angle));
        position[2] = target[2] + cos(glm::radians(this->angle));
    }
};