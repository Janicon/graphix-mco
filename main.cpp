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
#include "Classes/SpotLight.h"
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

    Model filter = Model("3D/plane.obj", "3D/filter.png",
        glm::vec3(0), 1.f, glm::vec3(0));
    player = Player("3D/fish.obj", "3D/brickwall.jpg",
        glm::vec3(0), 0.1f, glm::vec3(0, 0, 0),
        GL_RGB);
    Model sphere = Model("3D/ball.obj", "3D/ball.jpg",
        glm::vec3(0, 0, -10), 0.1f, glm::vec3(0),
        GL_RGB);

    Model elf = Model("3D/elf.obj", "3D/ball.jpg",
        glm::vec3(1, -2, -4), 0.01f, glm::vec3(0),
        GL_RGB);

    Model knife = Model("3D/knife.obj", "3D/knife.jpg",
        glm::vec3(2, -2, -2), 1.f, glm::vec3(0),
        GL_RGB);

    // Set player camera as default
    Camera activeCamera = (Camera)player.getActiveCamera();

    glEnable(GL_DEPTH_TEST);

    // Set callbacks
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, CursorCallback);

    ShaderManager filterShader = ShaderManager("filter");
    ShaderManager playerShader = ShaderManager("player");
    ShaderManager npcShader = ShaderManager("npc");
    ShaderManager lightShader = ShaderManager("lightSource");

    // TODO: Recheck recording if should be in model.initBuffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    DirectionLight directionLight = DirectionLight(
        glm::vec3(0, -5, 0), glm::vec3(1),
        .2f, glm::vec3(1),
        3.f, 25.f
    );
    glm::vec4 nvFilter = glm::vec4(0.05, 0.25, .05, 0.4);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Blend tester http://www.andersriggelsen.dk/glblendfunc.php
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        // Change active camera based on mode
        if (isTopDown)
            activeCamera = (Camera)orthoCam;
        else
            activeCamera = (Camera)player.getActiveCamera();

        /*** Draw skybox ***/
        if (player.isFPP() && !isTopDown) {
            skybox.resetFilterColor(nvFilter);
            skybox.draw(activeCamera.getViewMatrix(), 1);
        }
        else {
            skybox.resetFilterColor();
            skybox.draw(activeCamera.getViewMatrix(), 0);
        }

        /*** Draw player submarine ***/
        // Draw models
        playerShader.useShaderProgram();
        
        // Get position of active camera
        playerShader.sendVec3("cameraPos", activeCamera.getPosition());

        // Direction light variables
        playerShader.sendVec3("dirLight_direction", directionLight.getDirection());
        playerShader.sendVec3("dirLight_color", directionLight.getColor());
        playerShader.sendFloat("dirLight_strength", directionLight.getIntensity());
        playerShader.sendFloat("dirLight_ambientStr", directionLight.getAmbientStr());
        playerShader.sendVec3("dirLight_ambientColor", directionLight.getAmbientColor());
        playerShader.sendFloat("dirLight_specStr", directionLight.getSpecStr());
        playerShader.sendFloat("dirLight_specPhong", directionLight.getSpecPhong());
        
        // Draw object model
        playerShader.sendMat4("projection", activeCamera.getProjection());
        playerShader.sendMat4("view", activeCamera.getViewMatrix());

        player.getPlayer().draw(playerShader.getUniformLoc("transform"),
            playerShader.getUniformLoc("tex0"),
            playerShader.getUniformLoc("tex1"));
        
        /*** Draw relics ***/
        npcShader.useShaderProgram();

        // Get position of active camera
        npcShader.sendVec3("cameraPos", activeCamera.getPosition());

        // Direction light variables
        npcShader.sendVec3("dirLight_direction", directionLight.getDirection());
        npcShader.sendVec3("dirLight_color", directionLight.getColor());
        npcShader.sendFloat("dirLight_strength", directionLight.getIntensity());
        npcShader.sendFloat("dirLight_ambientStr", directionLight.getAmbientStr());
        npcShader.sendVec3("dirLight_ambientColor", directionLight.getAmbientColor());
        npcShader.sendFloat("dirLight_specStr", directionLight.getSpecStr());
        npcShader.sendFloat("dirLight_specPhong", directionLight.getSpecPhong());

        // Spot light variables
        npcShader.sendVec3("spotLight_position", player.getFlashlight().getPos());
        npcShader.sendVec3("spotLight_color", player.getFlashlight().getColor());
        npcShader.sendFloat("spotLight_linear", player.getFlashlight().getLinear());
        npcShader.sendFloat("spotLight_quadratic", player.getFlashlight().getQuadratic());
        npcShader.sendFloat("spotLight_ambientStr", player.getFlashlight().getAmbientStr());
        npcShader.sendVec3("spotLight_ambientColor", player.getFlashlight().getAmbientColor());
        npcShader.sendFloat("spotLight_specStr", player.getFlashlight().getSpecStr());
        npcShader.sendFloat("spotLight_specPhong", player.getFlashlight().getSpecPhong());

        npcShader.sendFloat("cutoff", glm::cos(glm::radians(7.5f)));
        npcShader.sendFloat("outercutoff", glm::cos(glm::radians(17.5f)));

        npcShader.sendInt("tex0", 0);
        npcShader.sendMat4("projection", activeCamera.getProjection());
        npcShader.sendMat4("view", activeCamera.getViewMatrix());

        if (player.isFPP() && !isTopDown) {
            npcShader.sendVec4("filterColor", nvFilter);
            npcShader.sendInt("isFPP", 1);
        }
        else
            npcShader.sendInt("isFPP", 0);
        sphere.draw(npcShader.getUniformLoc("transform"));

        /*** Draw filter in FPP mode ***/
        /*
        if (player.isFPP()) {
            // Set filter in front of camera
            glm::vec3 rot = player.getPlayer().getRotation();
            glm::vec3 pos = activeCamera.getPosition() + glm::vec3(
                0.5 * sin(glm::radians(rot.x)),
                0,
                0.5 * cos(glm::radians(rot.x)));;

            filter.setPosition(pos);
            filter.setRotation(rot);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_ALPHA);
            filterShader.useShaderProgram();

            filterShader.sendMat4("projection", activeCamera.getProjection());
            filterShader.sendMat4("view", activeCamera.getViewMatrix());
            filter.draw(filterShader.getUniformLoc("transform"));
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        */
        
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
    
    if (key == GLFW_KEY_2) {
        isTopDown = !isTopDown;


        //Update top view camera position and target to player
        if (isTopDown) {
            //get position of player 
            glm::vec3 playerPos = player.getPlayer().getPos();

            orthoCam.setPos(glm::vec3(playerPos.x, 1.f, playerPos.z));
            orthoCam.setTarget(glm::vec3(playerPos.x, 0, playerPos.z));
        }
    }

    if (!isTopDown) {
        //changes player position and camera view based on the key and action
        player.parseKey(key, action);
        return;
    }

    switch (key) {
    //Top View Camera Pan Controls
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
    static float sensitivity = 0.005;

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

    orthoCam.dragCamera(-sensitivity * (oldY - cursorY), -sensitivity * (oldX - cursorX));

    //changes third person camera view based on mouse position
    //player.parseCursor(window, xpos, ypos);
}