#pragma once
class Player {
private:
    enum cameraModes { FPP, TPP };
    // Distances: 100, 600, 3250
    float linear[3]{ 0.045, 0.007, 0.0014 };
    float quadratic[3]{ 0.0075, 0.0002, 0.000007 };

    Model obj;
    glm::vec3 objRotOffset = glm::vec3(180, 0, 0);
    PerspectiveCamera tpp = PerspectiveCamera(
        glm::vec3(0, 0, 1.5),
        glm::vec3(0),
        glm::vec3(0, 1, 0),
        true);
    PerspectiveCamera fpp = PerspectiveCamera(
        glm::vec3(0, 0, 1),
        glm::vec3(0),
        glm::vec3(0, 1, 0),
        false);
    PointLight flashlight;

    // User control
    cameraModes activeCamera = TPP;
    int lightLevel = 0;
    bool lookMode = false;
    double cursorX, cursorY;

    /* Repositions light in front of the rendered object */
    void repositionLight() {
        glm::vec3 newPos = obj.getPos();
        float distance = 4.25f;
        // Position the light to a point in a circle around the object's current position
        float facing = obj.getRotation().x - objRotOffset.x;
        newPos += glm::vec3(
            distance * sin(glm::radians(facing)),
            0.25f,
            distance * cos(glm::radians(facing)));

        flashlight.setPos(newPos);
    }

public:
    Player() {}

    Player(std::string objPath,
        std::string texPath, int texFormat,
        std::string normPath, int normFormat,
        glm::vec3 pos, float size, glm::vec3 rot)
    {
        // Create drawable model that uses normals
        obj = Model(objPath.c_str(),
            texPath.c_str(), texFormat,
            true, normPath, normFormat,
            pos, size, rot);
        obj.initBuffers();

        // Create submarine light
        flashlight = PointLight(
            glm::vec3(0), glm::vec3(1), // Pos to be overriden
            .1f, glm::vec3(1),
            1.f, 32.f
        );
        flashlight.setAttenuation(linear[lightLevel], quadratic[lightLevel]);
        repositionLight();

        // Set camera position behind the sub
        tpp.adjustCameraTpp(obj.getPos(), obj.getRotation() - objRotOffset);
    }

    /* Getters */
    bool isFPP() {
        return activeCamera == FPP;
    }
    Model getPlayer() {
        return obj;
    }
    PerspectiveCamera getActiveCamera() {
        if (activeCamera == FPP)
            return fpp;
        else
            return tpp;
    }
    PointLight getFlashlight() {
        return flashlight;
    }

    /* Methods */
    /* Parse keyboard input to control player */
    void parseKey(int key, int action) {
        // Register only key press and hold
        if (action == GLFW_RELEASE)
            return;
        // how fast the player will move
        static float speed = 0.5;
        switch (key) {
            // Change active camera
            case GLFW_KEY_1:
                if (activeCamera == FPP)
                    activeCamera = TPP;
                else
                    activeCamera = FPP;
                break;

            // Movement
            // WS - main object z position
            // QE - main object y position
            // AD - main object x rotation

            //ascends the player
            case GLFW_KEY_Q:
                if (!(obj.getPos().y > -0.1)) {
                    obj.modPos(glm::vec3(0, 1.f, 0));
                    cout << "Current Depth: " << obj.getPos().y << endl;
                }
                break;
            //go forward towards where the player is facing based on the x-axis rotation of the player
            //uses sine and cosine of player's x-axis to get the angle where it is heading to
            case GLFW_KEY_W:
                glm::vec3 rotation_w = obj.getRotation() - objRotOffset;
                obj.modPos(glm::vec3( 
                    speed * sin(glm::radians(rotation_w.x)), 
                    0,
                    speed * cos(glm::radians(rotation_w.x))));
                break;
            //descends the player
            case GLFW_KEY_E:
                obj.modPos(glm::vec3(0, -1.f, 0));
                cout << "Current Depth: " << obj.getPos().y << endl;

              
                break;
            //turns the player to the left
            case GLFW_KEY_A:
                obj.adjustRotate(glm::vec3(5.f, 0, 0));
                break;
            //go backward opposite where the player is facing based on the x-axis rotation of the player
            //uses sine and cosine of player's x-axis to get the angle where it is heading to
            case GLFW_KEY_S:
                glm::vec3 rotation_s = obj.getRotation() - objRotOffset;
                obj.modPos(glm::vec3(
                    -speed * sin(glm::radians(rotation_s.x)),
                    0,
                    -speed * cos(glm::radians(rotation_s.x))));
                break;
            //turns the player to the right
            case GLFW_KEY_D:
                obj.adjustRotate(glm::vec3(-5.f, 0, 0));
                break;

            // Change flashlight brightness
            case GLFW_KEY_F:
                cycleLight();
                break;
        }

        // Update camera positions
        tpp.adjustCameraTpp(obj.getPos(), obj.getRotation() - objRotOffset);
        fpp.adjustCameraFpp(obj.getPos(), obj.getRotation() - objRotOffset);

        // Move player light to front of player
        repositionLight();
    }

    /* Parse cursor input to control player camera */
    void parseCursor(GLFWwindow* window, double xpos, double ypos) {
        // Exit if camera is in orthographic view
        if (activeCamera == FPP)
            return;

        // X degrees per pixel
        static float sensitivity = 0.25;

        // Unlock freelook on mouse press, lock on release
        int mouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseButton == GLFW_PRESS && !lookMode) {
            lookMode = true;
            //resets pitch and yaw including player's x-axis rotation 
            tpp.setYawPitch(obj.getRotation() - objRotOffset);
            // Set starting position for cursor
            glfwGetCursorPos(window, &cursorX, &cursorY);
        }
        else if (mouseButton == GLFW_RELEASE) {
            //resets to default camera view after mouse release
            tpp.adjustCameraTpp(obj.getPos(), obj.getRotation() - objRotOffset);
            lookMode = false;
        }

        // Proceed if freelook is unlocked
        if (!lookMode) 
            return;

        // Move pitch and yaw depending on how far cursor moves
        double oldX = cursorX;
        double oldY = cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);

        //updates camera position based on mouse position
        tpp.revolve(sensitivity * (cursorX - oldX), sensitivity * (oldY - cursorY), obj.getPos());
    }

    /* Cycles light level of point light */
    void cycleLight() {
        lightLevel++;
        if (lightLevel > 2)
            lightLevel = 0;
        flashlight.setAttenuation(linear[lightLevel], quadratic[lightLevel]);
    }

    /* Deletion of buffers after object use */
    void cleanup() {
        obj.cleanup();
    }
};