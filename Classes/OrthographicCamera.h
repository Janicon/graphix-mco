#pragma once

class OrthographicCamera : public Camera {
private:
    // Set to top-view
    const glm::mat4 base_projection = glm::ortho(
        -15.0f, 15.0f,
        -15.0f, 15.0f,
        -0.1f, 1000.0f
    );

public:
    OrthographicCamera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
        this->position = position;
        this->target = target;
        this->worldUp = worldUp;
        projection = base_projection;
    }

    /* Methods */
    /* Moves camera in the XZ axis
    *  @param position - value to move XZ position of camera
    *       Note that position[1] should be zero
    */
    void panCamera(glm::vec3 position) {
        this->position += position;
        this->target += position;
    }

    /* Moves camera in the XZ axis
    *  @param xValue - Distance to pan in the X axis
    *  @param yValue - Distance to pan in the Z axis
    */
    void panCamera(float xValue, float yValue) {
        //Update camera X position and target
        this->position.x -= xValue;
        this->target.x -= xValue;

        //Update camera Z position and target
        this->position.z -= yValue;
        this->target.z -= yValue;
    }
};