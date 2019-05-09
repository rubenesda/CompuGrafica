// Transformation.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"

#include <iostream>
#include <string>

//GLEW
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>


//Other libs
#include <SOIL2/SOIL2.h>

//GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Other includes
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

//Define window dimension width and height
const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));


GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = WIDTH / 2.0f;

bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 6.0f, 8.0f);

unsigned int loadCubemap(vector<std::string> faces);

void configWindow()
{
	//Parameters of the Windows GLFW. Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLU_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLU_FALSE);
}


int main()
{
	//=================================================  TEMPLATE =============================================================
	//Initialize GLFW
	glfwInit();

	configWindow();

	//Create a GLFW window that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Mundo", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	//In case of error or we can't creating the window the script gives terminating GLFW
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	//Initialize GLEW to setup OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	//Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Enable alpha support
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	//========================================== TEMPLATE ===========================================

	// Setup and compile our shaders
	// Load models with textures
	//Shader shader("res/shaders/modelLoading.vs", "res/shaders/modelLoading.frag");  

	// Load models without textures only materials and color
	Shader shaderColor("res/shaders/lighting.vs", "res/shaders/lighting.frag");

	// Load models textures with light Maps
	//Shader shader("res/shaders/lightingMap.vs", "res/shaders/lightingMap.frag");

	// Load models textures with light Spot
	//Shader shader("res/shaders/lightingSpot.vs", "res/shaders/lightingSpot.frag");

	// Load models textures with light Point
	//Shader shader("res/shaders/lightingPoint.vs", "res/shaders/lightingPoint.frag");

	// Load models textures with light Directional
	//Shader shader("res/shaders/lightingDirectional.vs", "res/shaders/lightingDirectional.frag");

	// Load models textures with Normal maps
	Shader shader("res/shaders/lightingMapNormal.vs", "res/shaders/lightingMapNormal.frag");

	// Load the color of the lamps
	Shader lampShader("res/shaders/lamp.vs", "res/shaders/lamp.frag");

	// Load the SkyBox
	Shader skyboxShader("res/shaders/skybox.vs", "res/shaders/skybox.frag");

	// Load models
	//Model Model1("res/models/cajaTexPorDos/cajaPorDos.obj");
	//Model Model1("res/models/cajaTexturizada/caja.obj");
	//Model Model1("res/models/lagoOBJ/lago.obj");
	//Model Model1("res/models/CuboDoble/CuboDual.obj");
	//Model Model1("res/models/CuboMaps - copia/CubeMapas.obj");
	Model Model1("res/models/lagoTextOBJ/lagoText.obj");
	//Model Model1("res/models/nanosuit/nanosuit.obj");
	Model Model3("res/models/Sphere/esfera.obj");

	// Draw in wireframe
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


	// Creation of Lamp

	//Vertices
	// use with Perspective Projection

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};




	GLuint VBO;

	//Generate vertex array and buffers of the objects
	glGenBuffers(1, &VBO);

	GLuint lightVAO;
	//Generate vertex array and buffers of the objects
	glGenVertexArrays(1, &lightVAO);
	//To do Bind of vertex array
	glBindVertexArray(lightVAO);
	//Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Positions attributes in the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);//Unbind boxVAO

						 //Creation of the skybox
	float skyboxVertices[] = {
		// positions          
		-100.0f,  100.0f, -100.0f,
		-100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,

		-100.0f, -100.0f,  100.0f,
		-100.0f, -100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f,  100.0f,
		-100.0f, -100.0f,  100.0f,

		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,

		-100.0f, -100.0f,  100.0f,
		-100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f, -100.0f,  100.0f,
		-100.0f, -100.0f,  100.0f,

		-100.0f,  100.0f, -100.0f,
		100.0f,  100.0f, -100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		-100.0f,  100.0f,  100.0f,
		-100.0f,  100.0f, -100.0f,

		-100.0f, -100.0f, -100.0f,
		-100.0f, -100.0f,  100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		-100.0f, -100.0f,  100.0f,
		100.0f, -100.0f,  100.0f
	};


	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	int width, height;

	vector<std::string> faces
	{
		"res/images/skybox/right.jpg",
		"res/images/skybox/left.jpg",
		"res/images/skybox/top.jpg",
		"res/images/skybox/bottom.jpg",
		"res/images/skybox/front.jpg",
		"res/images/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);


	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	//Create a projection matrix 
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);


	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		lightPos.x -= 0.001f;
		lightPos.z -= 0.001f;

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		//Render
		//Clear color buffer
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Aqui uso el shader de Lighting Spot
		shader.use();	

		GLint lightPosLoc = glGetUniformLocation(shader.ID, "lightPos"); //Usar este para Normal Mapping
		//GLint lightPosLoc = glGetUniformLocation(shader.ID, "light.position"); //Usar este para iluminacion Spot y point
		GLint viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");

		// Declaracion de variables en caso de una Shader para luz tipo Spot - Descomentar en caso de usarlo
		GLint lightSpotdirLoc = glGetUniformLocation(shader.ID, "light.direction");
		GLint lightSpotCutOffLoc = glGetUniformLocation(shader.ID, "light.cutOff");
		GLint lightSpotOuterCutOffLoc = glGetUniformLocation(shader.ID, "light.outerCutOff");
		
		// Cuando se quiere que interactue con una luz puntual o desde un lugar especifico, quitar comentario
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		// Si quiere que interactue con la luz de tipo Spot en la camara, comentar el anterior y quitar el comentario de este.
		//glUniform3f(lightPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		//Setup para las variables en caso de un Shader para luz tipo Spot - Descomentar en caso de usarlo
		glUniform3f(lightSpotdirLoc, camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(12.5f)));
		glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(17.5f)));

		// Estandar, para cualquier Shader de iluminacion
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		//Ajustar propiedades de la luz
		glUniform3f(glGetUniformLocation(shader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shader.ID, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);

		//Ajustes de propiedades de la luz para tipo Point y Spot - Descomentar en caso de usarlo
		glUniform1f(glGetUniformLocation(shader.ID, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.ID, "light.linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader.ID, "light.quadratic"), 0.02f);


		glm::mat4 view = camera.GetviewMatrix();
		//glm::mat4 view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model 1
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Model1.Draw(shader);

		
		// Draw the loaded model 3 With Colors
		shaderColor.use();

		lightPosLoc = glGetUniformLocation(shaderColor.ID, "light.position"); //Si voy a usar la misma luz uso la misma variable, si va depender de otra luz crear otra variables Ejemplo "lightPosLoc2"
		viewPosLoc = glGetUniformLocation(shaderColor.ID, "viewPos");

		// Cuando se quiere que interactue con una luz puntual, quitar comentario
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		// Si quiere que interactue con la luz de tipo Spot, comentar el anterior y quitar comentario de este.
		//glUniform3f(lightPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		glUniform3f(glGetUniformLocation(shaderColor.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shaderColor.ID, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shaderColor.ID, "light.specular"), 1.0f, 1.0f, 1.0f);

		glUniformMatrix4fv(glGetUniformLocation(shaderColor.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderColor.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model1;
		model1 = glm::translate(model1, glm::vec3(5.0f, 5.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model1 = glm::scale(model1, glm::vec3(4.0f, 4.0f, 4.0f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shaderColor.ID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
		Model3.Draw(shaderColor);
		

		/*
		// Draw the loaded model 3
		glm::mat4 model1;
		model1 = glm::translate(model1, glm::vec3(-5.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model1 = glm::scale(model1, glm::vec3(1.0f, 1.0f, 1.0f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
		Model3.Draw(shader);
		*/

		// Draw Lamp

		//Render Lamp
		lampShader.use();

		GLint modelLoc = glGetUniformLocation(lampShader.ID, "model");
		GLint viewLoc = glGetUniformLocation(lampShader.ID, "view");
		GLint projLoc = glGetUniformLocation(lampShader.ID, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();

		GLint viewLoc2 = glGetUniformLocation(skyboxShader.ID, "view");
		GLint projLoc2 = glGetUniformLocation(skyboxShader.ID, "projection");

		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		//Swap screen buffers
		glfwSwapBuffers(window);

	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();


	glDeleteVertexArrays(1, &skyboxVAO);

	glDeleteBuffers(1, &skyboxVAO);

	return EXIT_SUCCESS;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	};
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	};
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	};
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	};
};


void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (GLFW_PRESS == action)
		{
			keys[key] = true;
		}
		else if (GLFW_RELEASE == action)
		{
			keys[key] = false;
		}
	}
};

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
};

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}