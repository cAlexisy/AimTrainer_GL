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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int loadTexture(const char* path);

// Juego

float meta[2] = { 62.0f, 72.0f };



// Posición de la Luz
glm::vec3 lightPos(1.2f, 5.0f, 2.0f);

// Luces
glm::vec4 luzNoche = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
glm::vec4 luzDia = glm::vec4(0.51f, 0.82f, 0.95f, 1.0f);
glm::vec4 luzEntorno = luzNoche;



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // shaders
    Shader lightingShader("shaders/project_casters.vs", "shaders/project_casters.fs");
    Shader lightCubeShader("shaders/project_light.vs", "shaders/project_light.fs");
    //Shader shader2d("shaders/project_2d.vs", "shaders/project_2d.fs");

    //modelos
    Model camp("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/shooting/camp.obj");
    Model weapon("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/weapon/weapon.obj");
    Model lamp("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/lamp/lamp.obj");
    ///Model lamp2("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/lamp/lamp.obj");
    Model ball("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/ball2/ball.obj");
    Model target("C:/Users/Bolivar/Documents/Visual Studio 2022/OpenGL/OpenGL/model/target/target.obj");
    

    

    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
        -0.98f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.6f, 0.6f,
        -0.98f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.6f, 0.4f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.4f, 0.4f,
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.4f, 0.6f,

         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.6f, 0.6f,
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.6f, 0.4f,
         0.98f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.4f, 0.4f,
         0.98f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.4f, 0.6f,

         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.6f, 0.6f,
         1.0f,  0.98f, 0.0f,   0.0f, 1.0f, 0.0f,   0.6f, 0.4f,
        -1.0f,  0.98f, 0.0f,   0.0f, 0.0f, 1.0f,   0.4f, 0.4f,
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.4f, 0.6f,

         1.0f,  -0.98f, 0.0f,   1.0f, 0.0f, 0.0f,   0.6f, 0.6f,
         1.0f,  -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.6f, 0.4f,
        -1.0f,  -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.4f, 0.4f,
        -1.0f,  -0.98f, 0.0f,   1.0f, 1.0f, 0.0f,   0.4f, 0.6f,



    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3 ,

        4,5,6,
        5,6,7,

        8,9,10,
        9,10,11,

        12,13,14,
        13,14,15,
    };

    
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        //glm::vec3(-1.0f, 0.35f, 0.7f),
        ///glm::vec3(1.0f, 0.35f, 0.7f),
        glm::vec3(2.4f, 0.35f, 0.7f),
        //glm::vec3(2.0f, 0.35f, 0.7f)
    };

    glm::vec3 spotLightPositions[] = {
            glm::vec3(-1.0f, 0.35f, -0.7f),

            glm::vec3(1.0f, 0.35f, -0.7f),

            glm::vec3(0.0f, 0.35f, -0.7f),

            glm::vec3(-1.0f, 0.35f, 1.85f),

            glm::vec3(1.0f, 0.35f, 1.85f),

            glm::vec3(0.0f, 0.35f, 1.85f),

    };

    


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glm::vec3 weaponOffset = glm::vec3(0.0f, -0.13f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(luzEntorno.x, luzEntorno.y, luzEntorno.z, luzEntorno.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        
        lightingShader.setFloat("material.shininess", 32.0f);
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

        
        // point light N
        string pT1 = "pointLights[";
        for (unsigned int i = 0; i < 15; i++) {
            lightingShader.setVec3(pT1 + std::to_string(i) + "].position", pointLightPositions[i]);
            lightingShader.setVec3(pT1 + std::to_string(i) + "].ambient", 0.15f, 0.15f, 0.15f);
            lightingShader.setVec3(pT1 + std::to_string(i) + "].diffuse", 0.9f, 0.9f, 0.9f);
            lightingShader.setVec3(pT1 + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(pT1 + std::to_string(i) + "].constant", 1.0f);
            lightingShader.setFloat(pT1 + std::to_string(i) + "].linear", 0.09);
            lightingShader.setFloat(pT1 + std::to_string(i) + "].quadratic", 0.032);

        }

        // point light N

        string pT2 = "spotLight[";
        for (unsigned int i = 0; i < 11; i++) {
            lightingShader.setVec3(pT2 + std::to_string(i) + "].position", spotLightPositions[i]);
            lightingShader.setVec3(pT2 + std::to_string(i) + "].direction", 0.0f, -1.0f, 0.0f);
            lightingShader.setVec3(pT2 + std::to_string(i) + "].ambient", 0.15f, 0.15f, 0.15f);
            lightingShader.setVec3(pT2 + std::to_string(i) + "].diffuse", 0.9f, 0.9f, 0.9f);
            lightingShader.setVec3(pT2 + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(pT2 + std::to_string(i) + "].constant", 1.0f);
            lightingShader.setFloat(pT2 + std::to_string(i) + "].linear", 0.09);
            lightingShader.setFloat(pT2 + std::to_string(i) + "].quadratic", 0.032);
            lightingShader.setFloat(pT2 + std::to_string(i) + "].cutOff", glm::cos(glm::radians(23.5f)));
            lightingShader.setFloat(pT2 + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(27.0f)));

        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            lightingShader.setVec3(pT2 + std::to_string(7) + "].position", camera.Position);
            lightingShader.setVec3(pT2 + std::to_string(7) + "].direction", camera.Front);
            lightingShader.setVec3(pT2 + std::to_string(7) + "].ambient", 0.15f, 0.15f, 0.15f);
            lightingShader.setVec3(pT2 + std::to_string(7) + "].diffuse", 0.9f, 0.9f, 0.9f);
            lightingShader.setVec3(pT2 + std::to_string(7) + "].specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(pT2 + std::to_string(7) + "].constant", 1.0f);
            lightingShader.setFloat(pT2 + std::to_string(7) + "].linear", 0.09);
            lightingShader.setFloat(pT2 + std::to_string(7) + "].quadratic", 0.032);
            lightingShader.setFloat(pT2 + std::to_string(7) + "].cutOff", glm::cos(glm::radians(12.5f)));
            lightingShader.setFloat(pT2 + std::to_string(7) + "].outerCutOff", glm::cos(glm::radians(15.0f)));
        }

        
        
        
        

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);


        //camp
        lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -0.3f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        lightingShader.setMat4("model", model);
        camp.Draw(lightingShader);
        
        // weapon
        lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);

        // Configura la matriz de modelo para el arma
        model = glm::mat4(1.0f);

        // Posición relativa a la cámara en su propio espacio
         // Ajusta para que esté en la posición correcta
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            weaponOffset = glm::vec3(0.0f, -0.09f, 0.0f); // Ajusta para que esté en la posición correcta
        }
        else {
            weaponOffset = glm::vec3(0.1f, -0.13f, 0.3f);
        }
        // Calcula la posición del arma en el espacio del mundo
        glm::vec3 weaponPosition = camera.Position
            + (camera.Right * weaponOffset.x)  // Movimiento a la derecha/izquierda
            + (camera.Up * weaponOffset.y)     // Movimiento arriba/abajo
            + (camera.Front * weaponOffset.z); // Movimiento adelante/atrás

        // Aplica la traslación
        model = glm::translate(model, weaponPosition);

        // Asegura que el arma rote con la cámara correctamente
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        rotationMatrix[0] = glm::vec4(camera.Right, 0.0f);
        rotationMatrix[1] = glm::vec4(camera.Up, 0.0f);
        rotationMatrix[2] = glm::vec4(-camera.Front, 0.0f); // -Front para corregir dirección
        rotationMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Aplica la rotación al modelo
        model = model * rotationMatrix;

        // Aplica la escala del arma (ajusta según el tamaño del modelo)
        model = glm::scale(model, glm::vec3(0.01f));

        // Si el arma está girada incorrectamente, ajusta la rotación final
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Pasa la matriz de modelo al shader
        lightingShader.setMat4("model", model);

        // Dibuja el arma
        weapon.Draw(lightingShader);



        //ball
        lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f));
        lightingShader.setMat4("model", model);
        ball.Draw(lightingShader);

        //target
        lightingShader.setVec3("dirLight.ambient", 2.0f, 0.0f, 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f));
        lightingShader.setMat4("model", model);
        target.Draw(lightingShader);


        lightCubeShader.use();
        lightCubeShader.setVec3("viewPos", camera.Position);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        lightCubeShader.setMat4("model", model);
        
        
        for (unsigned int i = 0; i < 1; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            lamp.Draw(lightCubeShader);
        }

        for (unsigned int i = 0; i < 5; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, spotLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            lamp.Draw(lightCubeShader);
        }
        

        



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
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



