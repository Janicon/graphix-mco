#pragma once
class Player {
private:
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

    std::vector<Camera*> persCameras{ &tpp, &fpp };

    // User control
    bool lightControl = false;
    int activeCamera = 0;
    bool lookMode = false;
    double cursorX, cursorY;

public:
    Player() {}

    Player(std::string objPath, std::string texPath,
        glm::vec3 pos, float size, glm::vec3 rot,
        int colorMode = GL_RGBA)
    {
        obj = Model(objPath.c_str(), texPath.c_str(),
            glm::vec3(0), .5f, glm::vec3(0, 0, -90),
            colorMode);
        obj.loadNorm("3D/brickwall_normal.jpg", GL_RGB);
        obj.setPivotObject();
    }

    Model getPlayer() {
        return obj;
    }

    Camera getActiveCamera() {
        if (activeCamera == 1)
            return fpp;
        else
            return tpp;
    }

    void setKey_Callback(int key,
        int action)
    {
        // Register only key press and hold
        if (action == GLFW_RELEASE)
            return;

        static float speed = 0.05;
        // Global controls
        switch (key) {
            // Change active camera
            // 1 - Perspective camera
            // 2 - Orthographic camera
        case GLFW_KEY_1:
            if (activeCamera == 1)
                activeCamera = 0;
            else
                activeCamera = 1;
            break;

            // Space - Change controlled object
        //case GLFW_KEY_2:
        //    if (lightControl)
        //        pointLight.setColor(glm::vec3(1));
        //    else
        //        pointLight.setColor(glm::vec3(.25f, 1, 1));
        //    lightControl = !lightControl;
        //    break;
        }

        if (activeCamera == 0)
            // Mode specific - object control
            switch (key) {
                //WS - main object z position
                //QE - main object y position
                //AD - main object x rotation
            case GLFW_KEY_Q:
                glm::vec3 objPos = obj.getPos();
                if (!(objPos.y > 0)) {
                    obj.modPos(glm::vec3(0, 0.1f, 0));
                    tpp.adjustCameraTpp(obj.getPos(), obj.getRotation()); //TODO: -1.49012e-08 0 DEPTH
                    cout << "Current Depth: " << objPos.y << endl;
                } break;
            case GLFW_KEY_W:
                glm::vec3 rotation_w = obj.getRotation();
                obj.modPos(glm::vec3(
                    -0.05f * sin(glm::radians(rotation_w.x)),
                    0,
                    -0.05f * cos(glm::radians(rotation_w.x))));
                tpp.adjustCameraTpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_E:
                obj.modPos(glm::vec3(0, -0.1f, 0));
                tpp.adjustCameraTpp(obj.getPos(), obj.getRotation());
                cout << "Current Depth: " << obj.getPos().y << endl; break;
            case GLFW_KEY_A:
                obj.adjustRotate(glm::vec3(1.f, 0, 0));
                tpp.adjustCameraTpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_S:
                glm::vec3 rotation_s = obj.getRotation();
                obj.modPos(glm::vec3(
                    0.05f * sin(glm::radians(rotation_s.x)),
                    0,
                    0.05f * cos(glm::radians(rotation_s.x))));
                tpp.adjustCameraTpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_D:
                obj.adjustRotate(glm::vec3(-1.f, 0, 0));
                tpp.adjustCameraTpp(obj.getPos(), obj.getRotation()); break;

            }
        else
            // Mode specific - object control
            switch (key) {
                //WS - main object z position
                //QE - main object y position
                //AD - main object x rotation
            case GLFW_KEY_Q:
                glm::vec3 objPos = obj.getPos();
                if (!(objPos.y > 0)) {
                    obj.modPos(glm::vec3(0, 0.1f, 0));
                    fpp.adjustCameraFpp(obj.getPos(), obj.getRotation());
                    cout << "Current Depth: " << objPos.y << endl;
                } break;
            case GLFW_KEY_W:
                glm::vec3 rotation_w = obj.getRotation();
                obj.modPos(glm::vec3(
                    -0.05f * sin(glm::radians(rotation_w.x)),
                    0,
                    -0.05f * cos(glm::radians(rotation_w.x))));
                fpp.adjustCameraFpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_E:
                obj.modPos(glm::vec3(0, -0.1f, 0));
                fpp.adjustCameraFpp(obj.getPos(), obj.getRotation());
                cout << "Current Depth: " << obj.getPos().y << endl; break;
            case GLFW_KEY_A:
                obj.adjustRotate(glm::vec3(1.f, 0, 0));
                fpp.adjustCameraFpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_S:
                glm::vec3 rotation_s = obj.getRotation();
                obj.modPos(glm::vec3(
                    0.05f * sin(glm::radians(rotation_s.x)),
                    0,
                    0.05f * cos(glm::radians(rotation_s.x))));
                fpp.adjustCameraFpp(obj.getPos(), obj.getRotation()); break;
            case GLFW_KEY_D:
                obj.adjustRotate(glm::vec3(-1.f, 0, 0));
                fpp.adjustCameraFpp(obj.getPos(), obj.getRotation()); break;

            }
    }

    void setCursorCallback(GLFWwindow* window, double xpos, double ypos) {
        // Exit if camera is in orthographic view
        if (activeCamera == 1)
            return;

        // X degrees per pixel
        static float sensitivity = 0.25;

        // Unlock freelook on mouse press, lock on release
        int mouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseButton == GLFW_PRESS && !lookMode) {
            lookMode = true;

            // Set starting position for cursor
            glfwGetCursorPos(window, &cursorX, &cursorY);
        }
        else if (mouseButton == GLFW_RELEASE)
            lookMode = false;

        // Proceed if freelook is unlocked
        if (!lookMode) {
            tpp.adjustCameraTpp(obj.getPos(), obj.getRotation());
            return;
        }


        // Move pitch and yaw depending on how far cursor moves
        double oldX = cursorX;
        double oldY = cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);

        tpp.revolve(sensitivity * (cursorX - oldX), sensitivity * (oldY - cursorY), obj.getPos());
    }
};