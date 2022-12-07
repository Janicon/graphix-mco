#pragma once

class OrthographicCamera : public Camera {
private:
    const glm::mat4 base_projection = glm::ortho(
        -2.0f, 2.0f,
        -2.0f, 2.0f,
        -0.1f, 100.0f
    );
    float angle = 90.f;

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

    void dragCamera(float yValue, float xValue) {

        this->position.x += xValue;
        this->target.x += xValue;

        this->position.z += yValue;
        this->target.z += yValue;

        
    }
};