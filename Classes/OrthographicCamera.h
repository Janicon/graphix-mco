#pragma once

class OrthographicCamera : public Camera {
private:
    const glm::mat4 base_projection = glm::ortho(
        -2.0f, 2.0f,
        -2.0f, 2.0f,
        -0.1f, 100.0f
    );

public:
    OrthographicCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        projection = base_projection;
    }
};