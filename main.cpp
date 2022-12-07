#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <string>
#include <iostream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Classes/Model.h"
#include "Classes/ShaderManager.h"
#include "Classes/Skybox.h"
#include "Classes/Camera.h"
#include "Classes/PerspectiveCamera.h"
#include "Classes/OrthographicCamera.h"
#include "Classes/Light.h"
#include "Classes/DirectionLight.h"
#include "Classes/PointLight.h"
#include "Classes/Player.h"

/* Global variables */
Player player;

OrthographicCamera orthoCam = OrthographicCamera(
    glm::vec3(0.f, 1, 0.f),
    glm::vec3(0),
    glm::vec3(0, 0, -1.f));

/* User controls */
bool isTopDown = false;
bool lookMode = false;
double cursorX, cursorY;

// Function declarations
void Key_Callback(GLFWwindow* window, int key, int scanCode, int action, int mods);
void CursorCallback(GLFWwindow* window, double xpos, double ypos);

int main(void)
{
    // Window
    GLFWwindow* window;
    float screenWidth = 720.f;
    float screenHeight = 720.f;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Jan Ambro Canicon", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // Initialize GLAD
    gladLoadGL();

    Skybox skybox = Skybox();
    stbi_set_flip_vertically_on_load(true);

    player = Player("3D/fish.obj", "3D/brickwall.jpg",
        glm::vec3(0), 0.1f, glm::vec3(0, 0, 0),
        GL_RGB);
    Model sphere = Model("3D/ball.obj", "3D/ball.jpg",
        glm::vec3(0, 0, -2), 0.01f, glm::vec3(0),
        GL_RGB);
    // Set player camera as default
    Camera activeCamera = (Camera)player.getActiveCamera();

    glEnable(GL_DEPTH_TEST);

    // Set callbacks
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, CursorCallback);

    ShaderManager modelShader = ShaderManager("model");
    ShaderManager lightShader = ShaderManager("lightSource");

    // TODO: Recheck recording if should be in model.initBuffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    DirectionLight directionLight = DirectionLight(
        //glm::vec3(4, 11, -3), glm::vec3(.33f, .1f, .66f),
        glm::vec3(0, 5, 0), glm::vec3(1),
        .2f, glm::vec3(1),
        3.f, 25.f
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Blend tester http://www.andersriggelsen.dk/glblendfunc.php
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);

        // Change active camera based on mode
        if (isTopDown)
            activeCamera = (Camera)orthoCam;
        else
            activeCamera = (Camera)player.getActiveCamera();

        // Draw skybox
        //skybox.draw(player.getActiveCamera().getViewMatrix());
        skybox.draw(activeCamera.getViewMatrix());

        // Draw models
        modelShader.useShaderProgram();

        // Get position of active camera
        modelShader.sendVec3("cameraPos", activeCamera.getPosition());

        // Direction light variables
        modelShader.sendVec3("dirLight_direction", directionLight.getDirection());
        modelShader.sendVec3("dirLight_color", directionLight.getColor());
        modelShader.sendFloat("dirLight_strength", directionLight.getIntensity());
        modelShader.sendFloat("dirLight_ambientStr", directionLight.getAmbientStr());
        modelShader.sendVec3("dirLight_ambientColor", directionLight.getAmbientColor());
        modelShader.sendFloat("dirLight_specStr", directionLight.getSpecStr());
        modelShader.sendFloat("dirLight_specPhong", directionLight.getSpecPhong());

        // Point light variables
        modelShader.sendVec3("pointLight_position", player.getFlashlight().getPos());
        modelShader.sendVec3("pointLight_color", player.getFlashlight().getColor());
        modelShader.sendFloat("pointLight_strength", player.getFlashlight().getIntensity());
        modelShader.sendFloat("pointLight_ambientStr", player.getFlashlight().getAmbientStr());
        modelShader.sendVec3("pointLight_ambientColor", player.getFlashlight().getAmbientColor());
        modelShader.sendFloat("pointLight_specStr", player.getFlashlight().getSpecStr());
        modelShader.sendFloat("pointLight_specPhong", player.getFlashlight().getSpecPhong());

        sphere.draw(modelShader.getUniformLoc("transform"));

        // Draw object model
        modelShader.sendMat4("projection", activeCamera.getProjection());
        modelShader.sendMat4("view", activeCamera.getViewMatrix());

        player.getPlayer().draw(modelShader.getUniformLoc("transform"),
            modelShader.getUniformLoc("tex0"),
            modelShader.getUniformLoc("tex1"));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Clean up variables
    player.cleanup();
    sphere.cleanup();
    skybox.cleanup();

    glfwTerminate();
    return 0;
}

void Key_Callback(GLFWwindow* window,
    int key,
    int scanCode,
    int action,
    int mods)
{
    // Register only key press and hold
    if (action == GLFW_RELEASE)
        return;
    
    if (key == GLFW_KEY_2)
        isTopDown = !isTopDown;

    if (!isTopDown) {
        player.parseKey(key, action);
        return;
    }
    
    switch (key) {
    //Top View Camera Pan Controls
        case GLFW_KEY_Q:
            orthoCam.dragCamera(-1.f);
            break;
        case GLFW_KEY_E:
            orthoCam.dragCamera(1.f);
            break;
        case GLFW_KEY_W:
            orthoCam.panCamera(glm::vec3(0, 0, -1));
            break;
        case GLFW_KEY_S:
            orthoCam.panCamera(glm::vec3(0, 0, 1));
            break;
        case GLFW_KEY_A:
            orthoCam.panCamera(glm::vec3(-1, 0, 0));
            break;
        case GLFW_KEY_D:
            orthoCam.panCamera(glm::vec3(1, 0, 0));
            break;
    }
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos) {
    // Send input to player
    if (!isTopDown)
    player.parseCursor(window, xpos, ypos);

    // X degrees per pixel
    static float sensitivity = 0.15;

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
        //tpp.adjustCameraTpp(obj.getPos(), obj.getRotation());
        return;
    }

    // Move pitch and yaw depending on how far cursor moves
    double oldX = cursorX;
    double oldY = cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    orthoCam.dragCamera(-sensitivity * (oldY - cursorY));
}