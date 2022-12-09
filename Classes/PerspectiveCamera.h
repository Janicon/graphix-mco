#pragma once

class PerspectiveCamera : public Camera {
private:

    //third person matrix. zFar is only 100 to implement nearsighted vision
    glm::mat4 base_projection_tpp = glm::perspective(
        glm::radians(60.f),
        1.f,
        .1f,
        100.f
    );
    //third person matrix. zFar is twice tpp to see much further
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

        // adds the delta yaw and pitch values to the default yaw and pitch
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

        //adds the position of the player to the current position of the camera
        position += playerpos;

        //sets the position of the player to the target of the camera
        target = playerpos;
    }

    void adjustCameraTpp(glm::vec3 playerpos, glm::vec3 playerrot) {
        // Distance from focal point
        static float distance = 1.5;

        //uses sine and cosine of player's x-axis to get the angle where it is heading to
        //then adds the position of the player to the current position of the camera
        position[0] = -sin(glm::radians(playerrot.x)) * distance + playerpos.x;
        position[1] = playerpos.y + 0.75f;
        position[2] = -cos(glm::radians(playerrot.x)) * distance + playerpos.z;

        //sets the position of the player to the target of the camera
        target = playerpos;
    }

    void adjustCameraFpp(glm::vec3 playerpos, glm::vec3 playerrot) {
        //get the current x-axis player rotation and offset it to face behind the object
        float newRot = playerrot.x - 180.f;

        //gets the offset for x and y axes using sine and cosine of newRot
        float offsetX = -sin(glm::radians(newRot));
        float offsetY = -cos(glm::radians(newRot));

        //sets the position of the player to the current position of the camera
        position = playerpos;

        //adds the x and y offsets to the current camera position
        position[0] += offsetX;
        position[2] += offsetY;

        //sets the camera position as the target of the camera
        target = position;

        //adds the x and y offsets to the current camera target
        target[0] += offsetX;
        target[2] += offsetY;
    }

    void setYawPitch(glm::vec3 playerrot) {
        //sets default pitch and yaw including where the player is facing
        pitch = -25.f;
        yaw = 90.f - playerrot.x;
    }

};