#pragma once

class PerspectiveCamera : public Camera {
private:
    // Third person projection. Small zFar for lower render distance
    glm::mat4 base_projection_tpp = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        100.f
    );
    // First person projection. Large zFar for higher render distance
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

    /* Methods */
    /* Revolves the camera around a position based on a pitch and yaw
    *  @param yawDelta - yaw distance to revolve
    *  @param pitchDelta - pitch distance to revolve
    *  @param curPos - specified position
    */
    void revolve(double yawDelta, double pitchDelta, glm::vec3 pos) {
        // Distance from focal point
        static float distance = 7.5;

        // adds the delta yaw and pitch values to the default yaw and pitch
        yaw += yawDelta;
        pitch += pitchDelta;

        // Limit pitch to straight up and down
        if (pitch > 89.f)
            pitch = 89.f;
        if (pitch < -89.f)
            pitch = -89.f;

        // Positions camera based on the pitch and yaw
        position[0] = -cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;
        position[1] = sin(glm::radians(pitch) * -1) * distance;
        position[2] = -sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * distance;

        // Moves the camera to the specified position
        position += pos;

        // Sets the target to the specified position
        target = pos;
    }

    /* Positions TPP camera
    *  @param pos - specified position
    *  @param rot - rotation to place camera
    */
    void adjustCameraTpp(glm::vec3 pos, glm::vec3 rot) {
        // Distance from focal point
        static float distance = 7.5;

        //uses sine and cosine of player's x-axis to get the angle where it is heading to
        //then adds the position of the player to the current position of the camera
        position[0] = -sin(glm::radians(rot.x)) * distance;
        position[1] = sin(glm::radians(-25.f) * -1) * distance;
        position[2] = -cos(glm::radians(rot.x)) * distance;

        position += pos;

        //sets the position of the player to the target of the camera
        target = pos;
    }

    /* Positions TPP camera
    *  @param pos - specified position
    *  @param rot - rotation to place camera
    */
    void adjustCameraFpp(glm::vec3 pos, glm::vec3 rot) {
        //get the current x-axis player rotation and offset it to face behind the object
        float newRot = rot.x - 180.f;

        //gets the offset for x and y axes using sine and cosine of newRot
        float offsetX = -sin(glm::radians(newRot));
        float offsetY = -cos(glm::radians(newRot));

        //sets the position of the player to the current position of the camera
        position = pos;

        //adds the x and y offsets to the current camera position
        position[0] += offsetX;
        position[2] += offsetY;

        //sets the camera position as the target of the camera
        target = position;

        //adds the x and y offsets to the current camera target
        target[0] += offsetX;
        target[2] += offsetY;
    }

    /* Sets yaw camera yaw and pitch given rotation values */
    void setYawPitch(glm::vec3 playerrot) {
        //sets default pitch and yaw including where the player is facing
        pitch = -25.f;
        yaw = 90.f - playerrot.x;
    }

};