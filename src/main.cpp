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
#include "TextRenderer.h"
#include "Renderer.h"
#include "LightingManager.h"
#include "ConfigLoader.h"

struct VoxelEngineConfig {
    bool frustumCullingEnabled;
    bool enableDepthTesting;
    bool enableFaceCulling;
    bool enableMSAA;
    bool enableWireframe;
};

VoxelEngineConfig loadVoxelEngineConfig(const std::string& fileName) {
    ConfigLoader& config = ConfigLoader::getInstance();

    if (!config.load(fileName)) {
        std::cerr << "Failed to load config file: " << fileName << std::endl;
        exit(1);
    }

    VoxelEngineConfig voxelConfig;
    voxelConfig.frustumCullingEnabled = config.get("VoxelEngine.frustumCullingEnabled", "False") == "True";
    voxelConfig.enableDepthTesting = config.get("VoxelEngine.enableDepthTesting", "False") == "True";
    voxelConfig.enableFaceCulling = config.get("VoxelEngine.enableFaceCulling", "False") == "True";
    voxelConfig.enableMSAA = config.get("VoxelEngine.enableMSAA", "False") == "True";
    voxelConfig.enableWireframe = config.get("VoxelEngine.enableWireframe", "False") == "True";

    return voxelConfig;
}

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

// Add after other global variables
bool showOutlines = true;
float outlineThickness = 0.03f;
glm::vec3 outlineColor(0.0f, 0.0f, 0.0f);


void update(float deltaTime, LightingManager& lightingManager)
{
    glfwPollEvents(); // poll events in the queue

    // make pointLights to move in a circle
    if (pointLights.size() > 0) {
        pointLights[0].position = lightPos; // Update first point light position
    }

    lightingManager.updateLightPosition(deltaTime);
}

void render(unsigned int VAO, unsigned int shaderProgram, World& world)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable polygon offset to prevent z-fighting
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glUseProgram(shaderProgram);

    // Set lighting uniforms
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));

    for (int i = 0; i < pointLights.size(); ++i) {
        std::string index = std::to_string(i);
        glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + index + "].position").c_str()), 1, glm::value_ptr(pointLights[i].position));
        glUniform3fv(glGetUniformLocation(shaderProgram, ("pointLights[" + index + "].color").c_str()), 1, glm::value_ptr(pointLights[i].color));
        glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + index + "].ambientStrength").c_str()), pointLights[i].ambientStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + index + "].diffuseStrength").c_str()), pointLights[i].diffuseStrength);
        glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + index + "].specularStrength").c_str()), pointLights[i].specularStrength);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "numPointLights"), pointLights.size());

    float currentAmbientStrength = ambientLightEnabled ? defaultAmbientStrength : 0.0f;
    float currentDiffuseStrength = diffuseLightEnabled ? diffuseStrength : 0.0f;
    float currentSpecularStrength = specularLightEnabled ? specularStrength : 0.0f;
    
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), currentAmbientStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), currentDiffuseStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), currentSpecularStrength);
    glUniform1i(glGetUniformLocation(shaderProgram, "shininess"), shininess);

    // Camera matrices
    glm::mat4 view = camera.GetViewMatrix();
    // Adjust near and far planes to reduce z-fighting
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.5f, 50.0f);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    Frustum frustum(projection, view);

    // Render each active voxel
    for (const auto& voxel : world.getActiveVoxels()) {
        glm::vec3 min = voxel.position - glm::vec3(0.5f);
        glm::vec3 max = voxel.position + glm::vec3(0.5f);

        if (!frustumCullingEnabled || frustum.isBoxInFrustum(min, max)) {
            // First pass: render the solid cube
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, voxel.position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            
            // Set material properties
            glUniform3fv(glGetUniformLocation(shaderProgram, "material.color"), 1, glm::value_ptr(voxel.material->color));
            glUniform1f(glGetUniformLocation(shaderProgram, "material.ambient"), voxel.material->ambient);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.diffuse"), voxel.material->diffuse);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.specular"), voxel.material->specular);
            glUniform1i(glGetUniformLocation(shaderProgram, "material.shininess"), voxel.material->shininess);
            glUniform1i(glGetUniformLocation(shaderProgram, "isOutline"), 0);
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Second pass: render the outline if enabled
            if (showOutlines) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                model = glm::scale(model, glm::vec3(1.0f + outlineThickness));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                
                // Set outline properties
                glUniform3fv(glGetUniformLocation(shaderProgram, "material.color"), 1, glm::value_ptr(outlineColor));
                glUniform1i(glGetUniformLocation(shaderProgram, "isOutline"), 1);
                
                glLineWidth(2.0f);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glLineWidth(1.0f);
            }
        }
    }

    // Render light source cubes
    for (const auto& light : pointLights) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.2f)); // Smaller cube for light source
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        // Set light cube color
        glVertexAttrib3f(2, light.color.r, light.color.g, light.color.b);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset to fill mode
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset to fill mode
}

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

unsigned int createTextShaderProgram() {
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
    
    out vec2 TexCoords;

    uniform mat4 projection;

    void main() {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
    }
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoords;
    out vec4 color;

    uniform sampler2D text;
    uniform vec3 textColor;

    void main() {    
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        color = vec4(textColor, 1.0) * sampled;
    }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

TextRenderer* textRenderer;

int gameLoop()
{
    VoxelEngineConfig config = loadVoxelEngineConfig("conf.ini");

    Renderer& renderer = Renderer::getInstance();

    renderer.initGLFW();
    renderer.setRenderVersion(3, 3);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Engine", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Set the framebuffer size callback
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    renderer.initGLAD();

    renderer.setViewPort(800, 600);

    if (config.enableDepthTesting) {
        renderer.enableDepthTesting();
    }
    if (config.enableFaceCulling) {
        renderer.enableFaceCulling();
    }
    if (config.enableMSAA) {
        renderer.enableMSAA();
    }

    unsigned int shaderProgram = createShaderProgram();
    
    unsigned int VAO = createCubeVAO();

    World world = generateWorld(128, 32, 128); // middle is height

    unsigned int textShaderProgram = createTextShaderProgram();
    textRenderer = new TextRenderer("fonts/Roboto-Light.ttf", 24, textShaderProgram);

    LightingManager lightingManager;
    lightingManager.setupPointLights();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    float frameRate = 60.0f;
    float frameTime = 1000.0f / frameRate;

    // Loops until window is closed & application is told to stop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        update(deltaTime, lightingManager);

        render(VAO, shaderProgram, world);

        glfwSwapBuffers(window);

        renderer.displayFPS();

        // MUST GO AT END OF GAME LOOP
        sleep(frameTime - (glfwGetTime() - currentFrame) * 1000.0);
    }
    glfwTerminate();
    return 0;
}

// Entry point
int main()
{
    return gameLoop();
}