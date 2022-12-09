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
    window = glfwCreateWindow(screenWidth, screenHeight, "No Man's Submarine", NULL, NULL);
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


    /*
        Player Positions near debris for testing:
        {0,0,-140}
        {80,-200,250},
        {-260,-240,415},
        {0,-450,800}

    */
    
    // Create player
    player = Player("3D/nemo.obj",
        "3D/nemo.png", GL_RGBA,
        "3D/nemo_normal.png", GL_RGBA,
        glm::vec3(0), 1.5f, glm::vec3(180.f, 0, 0));

    // Positions of enemy models
    float enemiesPos[6][3] =
    {
        {0.f,0.f,-150.f}, // Crab
        {120,-225,300}, // Dolphin
        {180,-275,380}, // Goldfish
        {-225,-195,375}, // Shark
        {-275,-260,435}, // Fish
        {0,-500,900} // Obelisk
    };

    // Scales of enemy models
    float enemiesSca[6] = {
        5.f,
        0.15f,
        1.5f,
        0.3f,
        2.5f,
        10.f
    };

    // Rotation of enemy models
    glm::vec3 enemiesRot[6] =
    {
        {-90,0,0}, // Crab
        {-120,-45,0}, // Dolphin 
        {45,-90,90}, // Goldfish
        {0,-90,0}, // Shark
        {-180,0,0}, // Fish
        {-90,0,0}// Obelisk
    };

    //Mesh file names and texture file names of enemy models
    std::string filenames[6][2] = {
        {"3D/crab.obj", "3D/crab.png"},
        {"3D/dolphin.obj", "3D/dolphin.jpg"},
        {"3D/goldfish.obj", "3D/goldfish.jpg"},
        {"3D/shark.obj", "3D/shark.jpg"},
        {"3D/fish.obj", "3D/bone.jpg"},
        {"3D/obelisk.obj", "3D/obelisk.jpg"}
    };
    
    //Vector array of enemies
    std::vector<Model> enemies;
    
    //insert crab model to enemies vector
    Model crab = Model(filenames[0][0],
        filenames[0][1], GL_RGBA,
        false, "", GL_RGB,
        glm::make_vec3(enemiesPos[0]), enemiesSca[0], enemiesRot[0]);
    crab.initBuffers();
    enemies.push_back(crab);

    //Push back remaining enemies
    Model model;
    for (int i = 0; i < 5; i++) {
        model = Model(filenames[i+1][0],
            filenames[i+1][1], GL_RGB,
            false, "", GL_RGB,
            glm::make_vec3(enemiesPos[i+1]), enemiesSca[i+1], enemiesRot[i+1]);
        model.initBuffers();
        enemies.push_back(model);
    }

    // Set player camera as default
    Camera activeCamera = (Camera)player.getActiveCamera();

    glEnable(GL_DEPTH_TEST);

    // Set callbacks
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, CursorCallback);

    // Create vertex and fragment shader managers
    ShaderManager filterShader = ShaderManager("filter");
    ShaderManager playerShader = ShaderManager("player");
    ShaderManager npcShader = ShaderManager("npc");
    ShaderManager lightShader = ShaderManager("lightSource");

    DirectionLight directionLight = DirectionLight(
        glm::vec3(0, -5, 0), glm::vec3(1),
        .2f, glm::vec3(1),
        3.f, 25.f
    );
    directionLight.setIntensity(0.5f);
    glm::vec4 nvFilter = glm::vec4(0.05, 0.25, .05, 0.4);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Change active camera based on mode
        if (isTopDown)
            activeCamera = (Camera)orthoCam;
        else
            activeCamera = (Camera)player.getActiveCamera();

        /*** Draw skybox ***/
        // Change filter color depending on perspective
        if (player.isFPP() && !isTopDown) {
            skybox.resetFilterColor(nvFilter);
            skybox.draw(activeCamera.getViewMatrix(), 1);
        }
        else {
            skybox.resetFilterColor();
            skybox.draw(activeCamera.getViewMatrix(), 0);
        }

        /*** Draw player submarine ***/
        // Set shader
        playerShader.useShaderProgram();
        
        // Get position of active camera
        playerShader.sendVec3("cameraPos", activeCamera.getPosition());

        // Direction light variables
        playerShader.sendVec3("directionLight.direction", directionLight.getDirection());
        playerShader.sendVec3("directionLight.color", directionLight.getColor());
        playerShader.sendFloat("directionLight.strength", directionLight.getIntensity());
        playerShader.sendFloat("directionLight.ambientStr", directionLight.getAmbientStr());
        playerShader.sendVec3("directionLight.ambientColor", directionLight.getAmbientColor());
        playerShader.sendFloat("directionLight.specStr", directionLight.getSpecStr());
        playerShader.sendFloat("directionLight.specPhong", directionLight.getSpecPhong());

        // Spot light variables
        playerShader.sendVec3("pointLight.position", player.getFlashlight().getPos());
        playerShader.sendVec3("pointLight.color", player.getFlashlight().getColor());
        playerShader.sendFloat("pointLight.linear", player.getFlashlight().getLinear());
        playerShader.sendFloat("pointLight.quadratic", player.getFlashlight().getQuadratic());
        playerShader.sendFloat("pointLight.ambientStr", player.getFlashlight().getAmbientStr());
        playerShader.sendVec3("pointLight.ambientColor", player.getFlashlight().getAmbientColor());
        playerShader.sendFloat("pointLight.specStr", player.getFlashlight().getSpecStr());
        playerShader.sendFloat("pointLight.specPhong", player.getFlashlight().getSpecPhong());

        // Draw object model
        playerShader.sendMat4("projection", activeCamera.getProjection());
        playerShader.sendMat4("view", activeCamera.getViewMatrix());

        // Draw player if in third-person view or in top view
        if(!player.isFPP() || isTopDown)
            player.getPlayer().draw(playerShader.getUniformLoc("transform"),
                playerShader.getUniformLoc("tex0"),
                playerShader.getUniformLoc("tex1"));
        
        /*** Draw debris (NPCs) ***/
        npcShader.useShaderProgram();

        // Get position of active camera
        npcShader.sendVec3("cameraPos", activeCamera.getPosition());

        // Direction light variables
        npcShader.sendVec3("directionLight.direction", directionLight.getDirection());
        npcShader.sendVec3("directionLight.color", directionLight.getColor());
        npcShader.sendFloat("directionLight.strength", directionLight.getIntensity());
        npcShader.sendFloat("directionLight.ambientStr", directionLight.getAmbientStr());
        npcShader.sendVec3("directionLight.ambientColor", directionLight.getAmbientColor());
        npcShader.sendFloat("directionLight.specStr", directionLight.getSpecStr());
        npcShader.sendFloat("directionLight.specPhong", directionLight.getSpecPhong());

        // Spot light variables
        npcShader.sendVec3("pointLight.position", player.getFlashlight().getPos());
        npcShader.sendVec3("pointLight.color", player.getFlashlight().getColor());
        npcShader.sendFloat("pointLight.linear", player.getFlashlight().getLinear());
        npcShader.sendFloat("pointLight.quadratic", player.getFlashlight().getQuadratic());
        npcShader.sendFloat("pointLight.ambientStr", player.getFlashlight().getAmbientStr());
        npcShader.sendVec3("pointLight.ambientColor", player.getFlashlight().getAmbientColor());
        npcShader.sendFloat("pointLight.specStr", player.getFlashlight().getSpecStr());
        npcShader.sendFloat("pointLight.specPhong", player.getFlashlight().getSpecPhong());

        npcShader.sendMat4("projection", activeCamera.getProjection());
        npcShader.sendMat4("view", activeCamera.getViewMatrix());

        // Change model color depending on perspective
        if (player.isFPP() && !isTopDown) {
            npcShader.sendVec4("filterColor", nvFilter);
            npcShader.sendInt("isFPP", 1);
        }
        else
            npcShader.sendInt("isFPP", 0);

        //Draw enemy models
        for (int i = 0; i < 6; i++)
            enemies[i].draw(npcShader.getUniformLoc("transform"),
                npcShader.getUniformLoc("tex0"));
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Clean up variables
    player.cleanup();

    //Cleanup enemy models
    for (int i = 0; i < 6; i++)
        enemies[i].cleanup();
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
    
    // Toggle top-down view
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

    // Sends movement input to player if camera is not in top-down view
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

        // Change flashlight brightness
        case GLFW_KEY_F:
            player.cycleLight();
            break;
    }
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos) {
    // Controls camera if not in top-down view
    if (!isTopDown)
        player.parseCursor(window, xpos, ypos);

    // X degrees per pixel
    static float sensitivity = 0.05;

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

    //Drag camera based on how far mouse moved from when left button is clicked
    orthoCam.panCamera(-sensitivity * (oldX - cursorX), -sensitivity * (oldY - cursorY));
}

