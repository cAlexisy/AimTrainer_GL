#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

// Callbacks y configuración
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Configuración global
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Modelos
Model camp, weapon, lamp, ball, target;

// Posición de la luz y entorno
glm::vec3 lightPos(1.2f, 5.0f, 2.0f);
glm::vec4 luzEntorno(0.3f, 0.3f, 0.3f, 1.0f);

// Posiciones de luces puntuales
glm::vec3 pointLightPositions[] = {
    glm::vec3(5.0f, 8.0f, 0.0f),
    glm::vec3(-5.0f, 0.0f, 0.0f),
    glm::vec3(-7.63f, 0.0f, 0.53f),
    glm::vec3(4.61f, 0.0f, 0.57f),
    glm::vec3(3.72f, 0.0f, 0.48f),
};

unsigned int loadTexture(const char* path, GLint filterMin = GL_LINEAR_MIPMAP_LINEAR, GLint filterMag = GL_LINEAR);

// Función para inicializar GLFW
GLFWwindow* initWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glEnable(GL_DEPTH_TEST);
    return window;
}

// Cargar modelos
void loadModels() {
    camp = Model("C:/Users/agust/OneDrive/Documentos/Visual Studio 2022/OpenGl/OpenGl/model/Shooting/camp.obj");
    weapon = Model("C:/Users/agust/OneDrive/Documentos/Visual Studio 2022/OpenGl/OpenGl/model/Weapon/weapon.obj");
    lamp = Model("C:/Users/agust/OneDrive/Documentos/Visual Studio 2022/OpenGl/OpenGl/model/Lamp/lamp.obj");
    ball = Model("C:/Users/agust/OneDrive/Documentos/Visual Studio 2022/OpenGl/OpenGl/model/ball2/ball.obj");
    target = Model("C:/Users/agust/OneDrive/Documentos/Visual Studio 2022/OpenGl/OpenGl/model/Target/target.obj");
}

// Configurar iluminación
void setupLights(Shader& shader) {
    shader.setVec3("viewPos", camera.Position);
    shader.setFloat("material.shininess", 32.0f);

    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

    unsigned int numPointLights = sizeof(pointLightPositions) / sizeof(glm::vec3);
    for (unsigned int i = 0; i < numPointLights; i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader.setVec3(base + ".position", pointLightPositions[i]);
        shader.setVec3(base + ".ambient", 0.15f, 0.15f, 0.15f);
        shader.setVec3(base + ".diffuse", 0.9f, 0.9f, 0.9f);
        shader.setVec3(base + ".specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat(base + ".constant", 1.0f);
        shader.setFloat(base + ".linear", 0.09);
        shader.setFloat(base + ".quadratic", 0.032);
    }
}

// Dibujar modelos
void drawModel(Shader& shader, Model& model, glm::vec3 position, glm::vec3 scale) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    shader.setMat4("model", modelMatrix);
    model.Draw(shader);
}

int main() {
    GLFWwindow* window = initWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL");
    if (!window) return -1;

    Shader lightingShader("shaders/project_casters.vs", "shaders/project_casters.fs");
    loadModels();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(luzEntorno.x, luzEntorno.y, luzEntorno.z, luzEntorno.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        setupLights(lightingShader);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        drawModel(lightingShader, camp, glm::vec3(1.0f, -0.3f, 0.0f), glm::vec3(0.3f));
        drawModel(lightingShader, weapon, glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.04f));
        drawModel(lightingShader, ball, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.06f));
        drawModel(lightingShader, target, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.06f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}