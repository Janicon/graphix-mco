#pragma once

class OrthographicCamera : public Camera {
private:

    const glm::mat4 base_projection = glm::ortho(
        -15.0f, 15.0f,
        -15.0f, 15.0f,
        -0.1f, 1000.0f
    );
    float angle = 90.f;

public:
    OrthographicCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        projection = base_projection;
    }

    //Pans the camera when using the WASD keys
    void panCamera(glm::vec3 position) {
        this->position += position;
        this->target += position;
    }

    //Drags the camera when using the mouse drag
    void dragCamera(float yValue, float xValue) {

        //Update camera X position and target
        this->position.x -= xValue;
        this->target.x -= xValue;

        //Update camera Z position and target
        this->position.z -= yValue;
        this->target.z -= yValue;

        
    }
};