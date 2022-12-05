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

Model obj, sphere;
std::vector<Model*> models{ &obj, &sphere };

DirectionLight directionLight;
PointLight pointLight;

// User control
bool lightControl = false;
int activeCamera = 0;
bool lookMode = false;
double cursorX, cursorY;

/* Function Declarations */
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
    obj = Model("3D/plane.obj", "3D/brickwall.jpg",
        glm::vec3(0), .5f, glm::vec3(0, 0, -90),
        GL_RGB);
    obj.loadNorm("3D/brickwall_normal.jpg", GL_RGB);
    obj.setPivotObject();
    sphere = Model("3D/ball.obj", "3D/ball.jpg",
        glm::vec3(0.f, 0.5f, 1.f), .0025f, glm::vec3(0),
        GL_RGB);

    player = Player("3D/plane.obj", "3D/brickwall.jpg",
        glm::vec3(0), .5f, glm::vec3(0, 0, -90),
        GL_RGB);


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

    pointLight = PointLight(
        glm::vec3(0), glm::vec3(1), // Pos to be overriden
        .1f, glm::vec3(1),
        1.f, 32.f
    );
    pointLight.setPos(sphere.getPos());
    directionLight = DirectionLight(
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
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        // Draw skybox
        skybox.draw(player.getActiveCamera().getViewMatrix(),
            player.getActiveCamera().getProjection());

        // Draw models
        modelShader.useShaderProgram();

        // Get position of active camera
        modelShader.sendVec3("cameraPos", player.getActiveCamera().getPosition());

        // Direction light variables
        modelShader.sendVec3("dirLight_direction", directionLight.getDirection());
        modelShader.sendVec3("dirLight_color", directionLight.getColor());
        modelShader.sendFloat("dirLight_strength", directionLight.getIntensity());
        modelShader.sendFloat("dirLight_ambientStr", directionLight.getAmbientStr());
        modelShader.sendVec3("dirLight_ambientColor", directionLight.getAmbientColor());
        modelShader.sendFloat("dirLight_specStr", directionLight.getSpecStr());
        modelShader.sendFloat("dirLight_specPhong", directionLight.getSpecPhong());

        // Point light variables
        modelShader.sendVec3("pointLight_position", pointLight.getPos());
        modelShader.sendVec3("pointLight_color", pointLight.getColor());
        modelShader.sendFloat("pointLight_strength", pointLight.getIntensity());
        modelShader.sendFloat("pointLight_ambientStr", pointLight.getAmbientStr());
        modelShader.sendVec3("pointLight_ambientColor", pointLight.getAmbientColor());
        modelShader.sendFloat("pointLight_specStr", pointLight.getSpecStr());
        modelShader.sendFloat("pointLight_specPhong", pointLight.getSpecPhong());

        // Draw object model
        modelShader.sendMat4("projection", player.getActiveCamera().getProjection());
        modelShader.sendMat4("view", player.getActiveCamera().getViewMatrix());

        player.getPlayer().draw(modelShader.getUniformLoc("transform"),
            modelShader.getUniformLoc("tex0"),
            modelShader.getUniformLoc("tex1"));

        // Draw light source model
        lightShader.useShaderProgram();
        
        lightShader.sendMat4("projection", player.getActiveCamera().getProjection());
        lightShader.sendMat4("view", player.getActiveCamera().getViewMatrix());
        lightShader.sendVec3("color", pointLight.getColor());
        
        sphere.draw(lightShader.getUniformLoc("transform"));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Clean up variables
    obj.cleanup();
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
    player.setKey_Callback(key, action);
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos) {
    player.setCursorCallback(window, xpos, ypos);
}