#pragma once
class Player {
private:
    enum cameraModes { FPP, TPP };
    float linear[3]{ 0.0014, 0.007, 0.014 };
    float quadratic[3]{ 0.000007, 0.0002, 0.0007 };

    Model obj;
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
    SpotLight flashlight;

    // User control
    cameraModes activeCamera = TPP;
    int lightLevel = 0;
    bool lookMode = false;
    double cursorX, cursorY;

    void repositionLight() {
        glm::vec3 newPos = obj.getPos();
        float facing = obj.getRotation().x;
        newPos += glm::vec3(
            0.5 * sin(glm::radians(facing)),
            0,
            0.5 * cos(glm::radians(facing)));
        flashlight.setPos(newPos);
    }

public:
    Player() {}

    Player(std::string objPath, std::string texPath,
        glm::vec3 pos, float size, glm::vec3 rot,
        int colorMode = GL_RGBA)
    {
        obj = Model(objPath.c_str(), texPath.c_str(),
            pos, size, rot,
            colorMode);
        obj.loadNorm("3D/brickwall_normal.jpg", GL_RGB);
        obj.setPivotObject();

        flashlight = SpotLight(
            glm::vec3(0), glm::vec3(1), // Pos to be overriden
            .1f, glm::vec3(1),
            1.f, 32.f
        );
        flashlight.setAttenuation(linear[lightLevel], quadratic[lightLevel]);
        repositionLight();
    }

    /* Getters */
    Model getPlayer() {
        return obj;
    }

    PerspectiveCamera getActiveCamera() {
        if (activeCamera == FPP)
            return fpp;
        else
            return tpp;
    }

    SpotLight getFlashlight() {
        return flashlight;
    }

    /* Methods */
    // TODO: Move light along with model
    void parseKey(int key, int action) {
        // Register only key press and hold
        if (action == GLFW_RELEASE)
            return;
        // how fast the player will move
        static float speed = 0.05;
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
                if (!(obj.getPos().y > 0)) {
                    obj.modPos(glm::vec3(0, 0.1f, 0));
                    cout << "Current Depth: " << obj.getPos().y << endl;
                }
                break;
            //go forward towards where the player is facing based on the x-axis rotation of the player
            //uses sine and cosine of player's x-axis to get the angle where it is heading to
            case GLFW_KEY_W:
                glm::vec3 rotation_w = obj.getRotation();
                obj.modPos(glm::vec3( 
                    speed * sin(glm::radians(rotation_w.x)), 
                    0,
                    speed * cos(glm::radians(rotation_w.x))));
                break;
            //descends the player
            case GLFW_KEY_E:
                obj.modPos(glm::vec3(0, -0.1f, 0));
                cout << "Current Depth: " << obj.getPos().y << endl;
                break;
            //turns the player to the left
            case GLFW_KEY_A:
                obj.adjustRotate(glm::vec3(1.f, 0, 0));
                break;
            //go backward opposite where the player is facing based on the x-axis rotation of the player
            //uses sine and cosine of player's x-axis to get the angle where it is heading to
            case GLFW_KEY_S:
                glm::vec3 rotation_s = obj.getRotation();
                obj.modPos(glm::vec3(
                    -speed * sin(glm::radians(rotation_s.x)),
                    0,
                    -speed * cos(glm::radians(rotation_s.x))));
                break;
            //turns the player to the right
            case GLFW_KEY_D:
                obj.adjustRotate(glm::vec3(-1.f, 0, 0));
                break;

            // Change flashlight brightness
            case GLFW_KEY_F:
                lightLevel++;
                flashlight.setAttenuation(linear[lightLevel], quadratic[lightLevel]);
                if (lightLevel > 2)
                    lightLevel = 0;
                break;
        }

        // Update camera positions
        tpp.adjustCameraTpp(obj.getPos(), obj.getRotation());
        fpp.adjustCameraFpp(obj.getPos(), obj.getRotation());

        // Move player light to front of player
        repositionLight();
    }

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
            tpp.setYawPitch(obj.getRotation());
            // Set starting position for cursor
            glfwGetCursorPos(window, &cursorX, &cursorY);
        }
        else if (mouseButton == GLFW_RELEASE) {
            //resets to default camera view after mouse release
            tpp.adjustCameraTpp(obj.getPos(), obj.getRotation());
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

    void cleanup() {
        obj.cleanup();
    }
};