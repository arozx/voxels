#include <glad/glad.h>
#include <GLFW/glfw3.h>

// standard headers
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

// glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// custom headers
#include "Camera.h"
#include "Input.h"
#include "shaders.h"
#include "World.h"
#include "Voxel.h"
#include "Frustum.h"

// set camera position above the terrain
Camera camera(glm::vec3(16.0f, 20.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f);
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Lighting variables
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float ambientStrength = 0.1f;

// Add after other global variables
bool ambientLightEnabled = true;
float defaultAmbientStrength = 0.1f;

// Add after other lighting variables
bool diffuseLightEnabled = true;
float defaultDiffuseStrength = 1.0f;
float diffuseStrength = defaultDiffuseStrength;

// Add after other lighting variables
float lightRadius = 10.0f;
float lightHeight = 10.0f;
float lightAngle = 0.0f;

// Add after other lighting variables
bool specularLightEnabled = true;
float specularStrength = 0.5f;
int shininess = 32;  // Controls how focused the specular highlight is

// Global variable to control frustum culling
bool frustumCullingEnabled = true;

// gets from glfw in milliseconds
double getCurrentTime()
{
    return glfwGetTime() * 1000.0;
}

void update()
{
    // Update game state
}

void updateLightPosition(float deltaTime) {
    // Make light orbit around the center
    lightAngle += deltaTime * 0.5f;
    lightPos.x = sin(lightAngle) * lightRadius + 16.0f;
    lightPos.z = cos(lightAngle) * lightRadius + 16.0f;
    lightPos.y = lightHeight;
}

void render(unsigned int VAO, unsigned int shaderProgram, World& world)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    glUseProgram(shaderProgram);

    // Set lighting uniforms
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));
    
    float currentAmbientStrength = ambientLightEnabled ? defaultAmbientStrength : 0.0f;
    float currentDiffuseStrength = diffuseLightEnabled ? diffuseStrength : 0.0f;
    float currentSpecularStrength = specularLightEnabled ? specularStrength : 0.0f;
    
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), currentAmbientStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), currentDiffuseStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), currentSpecularStrength);
    glUniform1i(glGetUniformLocation(shaderProgram, "shininess"), shininess);

    // Camera matrices
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    Frustum frustum(projection, view);

    // Render each active voxel
    for (const auto& voxel : world.getActiveVoxels()) {
        glm::vec3 min = voxel.position - glm::vec3(0.5f);
        glm::vec3 max = voxel.position + glm::vec3(0.5f);

        if (!frustumCullingEnabled || frustum.isBoxInFrustum(min, max)) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, voxel.position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            
            // Set material properties
            glUniform3fv(glGetUniformLocation(shaderProgram, "material.color"), 1, glm::value_ptr(voxel.material->color));
            glUniform1f(glGetUniformLocation(shaderProgram, "material.ambient"), voxel.material->ambient);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.diffuse"), voxel.material->diffuse);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.specular"), voxel.material->specular);
            glUniform1i(glGetUniformLocation(shaderProgram, "material.shininess"), voxel.material->shininess);
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // Render light source cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // Smaller cube for light source
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
    // Set light cube color (bright white)
    glVertexAttrib3f(2, 1.0f, 1.0f, 1.0f);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset to fill mode
}

// sleep for a given number of milliseconds
void sleep(double milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(milliseconds)));
}

unsigned int createShaderProgram()
{
    // Build and compile the vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Build and compile the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int createCubeVAO()
{
    float vertices[] = {
        // positions          // normals
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return VAO;
}

unsigned int generateRandomSeed() {
    // Combine hardware random device with time-based seed
    std::random_device rd;
    auto time = std::chrono::high_resolution_clock::now();
    auto time_seed = time.time_since_epoch().count();
    
    // Mix the seeds using XOR
    return rd() ^ static_cast<unsigned int>(time_seed);
}

// Game loop
int gameLoop()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram();
    unsigned int VAO = createCubeVAO();

    // Create world and generate terrain
    World world(32, 32, 32); // Bigger world for better terrain
    unsigned int seed = generateRandomSeed();
    std::cout << "Generated terrain with seed: " << seed << std::endl;
    
    // Choose a random terrain type
    World::TerrainType terrainTypes[] = {
        World::FLAT, World::HILLS, 
        World::MOUNTAINS, World::ISLANDS
    };
    int randomType = seed % 4;
    world.generateTerrain(seed, terrainTypes[randomType]);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);
        updateLightPosition(deltaTime);
        update();
        render(VAO, shaderProgram, world);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Sleep to keep frame rate constant: FRAME_TIME = 1/FPS
        const float FRAME_TIME = 16.6667;
        sleep(FRAME_TIME - (glfwGetTime() - currentFrame) * 1000.0);
    }

    glfwTerminate();
    return 0;
}

// Entry point
// Have main function as a wrapper for gameLoop
int main()
{
    return gameLoop();
}