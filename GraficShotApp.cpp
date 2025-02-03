//link git hub: https://github.com/cAlexisy/2024B_GR1CC_GR1/edit/main

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

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Inicialización de la cámara con una posición inicial en el mundo 3D
Camera camera(glm::vec3(3.28f, 0.2f, -2.25f));

// Variables para manejar la entrada del ratón
float lastX = SCR_WIDTH / 2.0f;  // Última posición del cursor en X (mitad de la pantalla)
float lastY = SCR_HEIGHT / 2.0f; // Última posición del cursor en Y (mitad de la pantalla)
bool firstMouse = true;          // Controla si es la primera vez que se mueve el ratón

// Variables para el control del tiempo entre fotogramas
float deltaTime = 0.0f;  // Tiempo entre el fotograma actual y el anterior
float lastFrame = 0.0f;  // Tiempo del último fotograma

// Declaración de la función para cargar texturas
unsigned int loadTexture(const char* path);

// Límites dentro del campo de juego
const float minX = -1.36f, maxX = 3.29f; // Límites en el eje X
const float minZ = -2.26f, maxZ = 2.0f;  // Límites en el eje Z
const float minY = 0.2f, maxY = 0.2f;    // Altura fija en el eje Y

// Límites de movimiento de la esfera (nuevos valores)
const float minX_sphere = -1.35f, maxX_sphere = -1.22f; // Rango permitido en X
const float minY_sphere = -0.3f, maxY_sphere = 0.1f;     // Altura fija en Y
const float minZ_sphere = -0.63f, maxZ_sphere = 1.9f;   // Rango permitido en Z

// Variable para indicar si el arma ha sido soltada o está en la mano del jugador
bool weaponDropped = false;

// Posición en la que se soltó el arma
glm::vec3 weaponDropPosition;

// Inicializar la posición de la esfera dentro del rango permitido al inicio del programa
glm::vec3 spherePosition(
	minX_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX_sphere - minX_sphere))),
	minY_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY_sphere - minY_sphere))), // Altura
	minZ_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ_sphere - minZ_sphere)))
);

// Variables para tamaño y velocidad del tarjet
float xMin = 3, xMax = 3.37f;  // Límites en el eje X
float yMin = -0.3f, yMax = 0.0f;    // Límites en el eje Y
float zMin = -0.57f, zMax = 2.0f;  // Límites en el eje Z

// Velocidades y direcciones de movimiento
float xSpeed = 0.6f, ySpeed = 0.4f, zSpeed = 0.5f;  // Velocidades de movimiento (ajustables)
float xDirection = 2.0f, yDirection = 0.0f, zDirection = 1.0f; // Direcciones iniciales
float currentX = xMin, currentY = yMin, currentZ = zMin;  // Comienza desde los límites mínimos

// Controla si se ha realizado un disparo
bool shotFired = false;

// Variable auxiliar para evitar que se active múltiples veces en una sola pulsación de tecla
bool gKeyPressed = false;

// Posición de la luz en la escena
glm::vec3 lightPos(1.2f, 5.0f, 2.0f);

// Definición de luces para diferentes momentos del día
glm::vec4 luzNoche = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);  // Luz tenue para la noche
glm::vec4 luzDia = glm::vec4(0.51f, 0.82f, 0.95f, 1.0f); // Luz brillante para el día

// Estado inicial de la iluminación en la escena
glm::vec4 luzEntorno = luzNoche;

// Limitar el número de desplazamientos del scroll para evitar cambios bruscos
int scrollCount = 0;
const int maxScrolls = 20;

// Variables para la mira
unsigned int crosshairVAO, crosshairVBO;

//Función para configurar la mira (crosshair)
void setupCrosshair() {
	float crosshairVertices[] = {
		// Línea horizontal
		-0.02f,  0.0f,  0.0f,
		 0.02f,  0.0f,  0.0f,

		 // Línea vertical
		  0.0f, -0.02f,  0.0f,
		  0.0f,  0.02f,  0.0f
	};

	glGenVertexArrays(1, &crosshairVAO);
	glGenBuffers(1, &crosshairVBO);

	glBindVertexArray(crosshairVAO);
	glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Función para renderizar la mira
void renderCrosshair(Shader& shader) {
	glDisable(GL_DEPTH_TEST); // Evitar que la mira se vea afectada por la profundidad

	shader.use();

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	shader.setMat4("projection", projection);

	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);

	glBindVertexArray(crosshairVAO);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST); // Restaurar la prueba de profundidad
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	bool isPlayingSound = false;

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

	glEnable(GL_DEPTH_TEST);

	// shaders
	Shader lightingShader("shaders/project_casters.vs", "shaders/project_casters.fs");
	Shader lightCubeShader("shaders/project_light.vs", "shaders/project_light.fs");
	Shader crosshairShader("shaders/shader_crosshair.vs", "shaders/shader_crosshair.fs");

	//modelos
	Model camp("C:/Users/User/Documents/Visual Studio 2022/OpenGL/OpenGL/model/Shooting/camp.obj");
	Model weapon("C:/Users/User/Documents/Visual Studio 2022/OpenGL/OpenGL/model/Weapon/weapon.obj");
	Model lamp("C:/Users/User/Documents/Visual Studio 2022/OpenGL/OpenGL/model/Lamp/lamp.obj");
	Model target("C:/Users/User/Documents/Visual Studio 2022/OpenGL/OpenGL/model/Target/target.obj");
	Model sphere("C:/Users/User/Documents/Visual Studio 2022/OpenGL/OpenGL/model/sphere/bola.obj");

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

	//Inicializar la mira
	setupCrosshair();

	glm::vec3 weaponOffset = glm::vec3(0.0f, -0.13f, 0.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!isPlayingSound) {
				// Reproducir el sonido en bucle mientras el botón está presionado
				if (!PlaySound(TEXT("./sound/gun.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP)) {
					std::cerr << "Error al reproducir el sonido." << std::endl;
				}
				isPlayingSound = true;  // Marcar que el sonido se está reproduciendo
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			// Detener el sonido cuando el botón se suelta
			PlaySound(NULL, 0, SND_PURGE);  // Esto detiene la reproducción del sonido
			isPlayingSound = false;  // Permitir que el sonido se reproduzca nuevamente
		}

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

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !shotFired)
		{
			// Generar una nueva posición aleatoria dentro de los límites permitidos
			float newX = minX_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX_sphere - minX_sphere)));
			float newY = minY_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY_sphere - minY_sphere))); // Altura aleatoria
			float newZ = minZ_sphere + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ_sphere - minZ_sphere)));

			// Asignar la nueva posición dentro del rango permitido
			spherePosition.x = newX;
			spherePosition.y = newY; // Ahora la altura también varía
			spherePosition.z = newZ;

			shotFired = true;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			shotFired = false;
		}

		// weapon
		lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);

		// Configura la matriz de modelo para el arma
		model = glm::mat4(1.0f);

		if (!weaponDropped) {
			// Renderizado en mano: el arma se mueve con la c�mara.
			glm::mat4 model = glm::mat4(1.0f);
			glm::vec3 weaponOffset;

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
				weaponOffset = glm::vec3(0.0f, -0.09f, 0.0f);
			else
				weaponOffset = glm::vec3(0.1f, -0.13f, 0.3f);

			// Posici�n relativa a la c�mara.
			glm::vec3 weaponPosition = camera.Position
				+ (camera.Right * weaponOffset.x)
				+ (camera.Up * weaponOffset.y)
				+ (camera.Front * weaponOffset.z);

			// Mantener el arma en su posici�n calculada sin modificar el eje Y en este caso
			model = glm::translate(model, weaponPosition);

			// Rotar el arma para que se alinee con la c�mara.
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			rotationMatrix[0] = glm::vec4(camera.Right, 0.0f);
			rotationMatrix[1] = glm::vec4(camera.Up, 0.0f);
			rotationMatrix[2] = glm::vec4(-camera.Front, 0.0f);
			model = model * rotationMatrix;

			// Escalar y ajustar la rotaci�n para un tama�o adecuado
			model = glm::scale(model, glm::vec3(0.01f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// Aplicar la transformaci�n al shader de iluminaci�n
			lightingShader.setMat4("model", model);
			weapon.Draw(lightingShader);
		}
		else {
			// Renderizado del arma soltada: permanece en la posici�n donde estaba la c�mara al presionar G.
			glm::mat4 model = glm::mat4(1.0f);

			// Se utiliza la posici�n guardada en weaponDropPosition.
			// Asegurarse de ajustar el eje Y a 0.25 cuando el arma est� en el suelo.
			weaponDropPosition.y = -0.28f;

			model = glm::translate(model, weaponDropPosition);

			// Ajustar escala y rotaci�n para que se vea natural en el suelo.
			model = glm::scale(model, glm::vec3(0.01f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			// Aplicar la transformaci�n al shader de iluminaci�n
			lightingShader.setMat4("model", model);
			weapon.Draw(lightingShader);
		}

		//target
		currentX += xSpeed * xDirection * deltaTime;  // Actualiza la posición en X
		currentY += ySpeed * yDirection * deltaTime;  // Actualiza la posición en Y
		currentZ += zSpeed * zDirection * deltaTime;  // Actualiza la posición en Z
		// Cambiar la dirección en X cuando se alcanza el límite
		if (currentX >= xMax) {
			xDirection = -1.0f;
		}
		else if (currentX <= xMin) {
			xDirection = 1.0f;
		}
		// Cambiar la dirección en Y cuando se alcanza el límite
		if (currentY >= yMax) {
			yDirection = -1.0f;
		}
		else if (currentY <= yMin) {
			yDirection = 1.0f;
		}
		// Cambiar la dirección en Z cuando se alcanza el límite
		if (currentZ >= zMax) {
			zDirection = -1.0f;
		}
		else if (currentZ <= zMin) {
			zDirection = 1.0f;
		}
		model = glm::mat4(1.0f);  // Matriz de identidad
		model = glm::translate(model, glm::vec3(currentX, currentY, currentZ));  // Traslación
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotación de 90 grados alrededor del eje Y
		model = glm::scale(model, glm::vec3(0.06f));  // Escalado si es necesario
		lightingShader.setMat4("model", model);  // Enviar la matriz al shader
		// Dibuja el objeto - ball
		target.Draw(lightingShader);

		//sphere
		lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, spherePosition);
		model = glm::scale(model, glm::vec3(0.2f));
		lightingShader.setMat4("model", model);
		sphere.Draw(lightingShader);


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
		//Dibujar la mira
		renderCrosshair(crosshairShader);
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
	// Cerrar la ventana con Escape.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Detectar la pulsaci�n de la tecla G.
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyPressed) {
		if (!weaponDropped) {
			// Al presionar G, se guarda la posici�n actual de la c�mara.
			weaponDropPosition = camera.Position;
			weaponDropped = true;
		}
		else {
			// Si se quiere permitir recoger el arma, se podr�a alternar el estado.
			weaponDropped = false;
		}
		gKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		gKeyPressed = false;
	}

	// Procesamiento de movimiento de la c�mara (W, A, S, D, etc.)
	glm::vec3 newPosition = camera.Position;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		newPosition += glm::vec3(camera.Front.x, 0.0f, camera.Front.z) * camera.MovementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		newPosition -= glm::vec3(camera.Front.x, 0.0f, camera.Front.z) * camera.MovementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		newPosition -= glm::normalize(glm::cross(glm::vec3(camera.Front.x, 0.0f, camera.Front.z), camera.Up)) * camera.MovementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		newPosition += glm::normalize(glm::cross(glm::vec3(camera.Front.x, 0.0f, camera.Front.z), camera.Up)) * camera.MovementSpeed * deltaTime;

	// Aplicar restricciones al movimiento (ejemplo: mantener dentro de ciertos l�mites).
	if (newPosition.x >= minX && newPosition.x <= maxX &&
		newPosition.z >= minZ && newPosition.z <= maxZ)
	{
		camera.Position.x = newPosition.x;
		camera.Position.z = newPosition.z;
	}
	// Fijar la altura de la camara
	camera.Position.y = minY;

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

	// Permitir el scroll hacia abajo (zoom out) incluso si hemos alcanzado el m�ximo de 20 desplazamientos hacia arriba
	if (yoffset < 0)
	{
		camera.ProcessMouseScroll(yoffset);  // Procesar el desplazamiento del scroll hacia abajo
		if (scrollCount > 0)  // Decrementar el contador si es mayor que 0
		{
			scrollCount--;
		}
	}
	// Permitir el scroll hacia arriba (zoom in) solo si no hemos alcanzado el m�ximo de 20 desplazamientos
	else if (scrollCount < maxScrolls)
	{
		camera.ProcessMouseScroll(yoffset);  // Procesar el desplazamiento del scroll hacia arriba
		scrollCount++;  // Incrementar el contador de desplazamientos<<
	}
}
