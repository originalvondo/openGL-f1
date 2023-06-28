#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>	
#include <fstream>
#include <string>	
#include <sstream>
#include <vector>
#include "stb_image.h"	
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>	
#include <gtc\type_ptr.hpp>	

// global variables
#define SCR_WIDTH 900
#define SCR_HEIGHT 600

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -0.5f);
glm::vec3 cameraLookingAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// light position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse variables
float lastXpos = (float)(SCR_WIDTH / 2);
float lastYpos = (float)(SCR_HEIGHT/ 2);
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;

//function prototypes
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void processKeyboardInput(GLFWwindow* window) {
	float cameraSpeed = 5.0f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * cameraLookingAt;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * cameraLookingAt;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition += cameraSpeed * glm::cross(cameraUp, cameraLookingAt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * glm::cross(cameraUp, cameraLookingAt);

	// for light movement
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.z -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.z += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.x += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.x -= cameraSpeed;

}	
void processMouseInput(GLFWwindow* window, double xPosition, double yPosition) {
	if (firstMouse) {
		lastXpos = xPosition;
		lastYpos = yPosition;
		firstMouse = false;
	}

	float mouseSensitivity = 0.1f;

	// calculating the offset
	float xOffset = xPosition - lastXpos;
	float yOffset = lastYpos - yPosition;

	// setting the current mouse position to the last mouse position
	lastXpos = xPosition;
	lastYpos = yPosition;

	// multiplying with the mouse sensitivity 
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// checking if pitch is > 89.0f
	if (pitch > 88.0f)
		pitch = 88.0f;
	if (pitch < -88.0f)
		pitch = -88.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraLookingAt = glm::normalize(direction);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "kire", nullptr, nullptr);
	if (window == NULL) 
	{
		std::cout << "ERROR::WINDOW::CREATION" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glEnable(GL_DEPTH_TEST);
	glfwSetCursorPosCallback(window, processMouseInput);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// vertices
	float vertices[] = {
		// position              // texture coordinate   // normal
		-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,             0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,      1.0f, 0.0f,             0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,             0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,             0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,      0.0f, 1.0f,             0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,      0.0f, 0.0f,             0.0f, 0.0f, -1.0f,
							     
		-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,             0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,      1.0f, 0.0f,             0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 1.0f,             0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 1.0f,             0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,      0.0f, 1.0f,             0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,             0.0f, 0.0f, 1.0f,
							     
		-0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,       1.0f, 1.0f,            -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,             -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             -1.0f, 0.0f, 0.0f,
							     
		 0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,             1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,      0.0f, 0.0f,             1.0f, 0.0f, 0.0f,
	 	 0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             1.0f, 0.0f, 0.0f,
							     
		-0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,      1.0f, 1.0f,             0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,      1.0f, 0.0f,             0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,      1.0f, 0.0f,             0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,      0.0f, 0.0f,             0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,      0.0f, 1.0f,             0.0f, -1.0f, 0.0f,
							     
		-0.5f,  0.5f, -0.5f,      0.0f, 1.0f,             0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,      1.0f, 1.0f,             0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 0.0f,             0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,      0.0f, 0.0f,             0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,      0.0f, 1.0f,             0.0f, 1.0f, 0.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  -5.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// buffers for cube
	GLuint cubeVao;
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	GLuint cubeVbo;
	glGenBuffers(1, &cubeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// texture for cube
	GLuint texture1;
	int width, height, nrChannels;
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	unsigned char* texture1data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	std::cout << "width: " << width << "\nheight: " << height << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// buffer configuration
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	// buffers for light
	GLuint lightVao;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);

	GLuint lightVbo;
	glGenBuffers(1, &lightVbo);
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	// shader program
	GLuint cubeShaderProgram = LoadShaders("default.vert", "default.frag");
	GLuint lightShaderProgram = LoadShaders("light.vert", "light.frag");

	// setting texture units
	glUniform1i(glGetUniformLocation(cubeShaderProgram, "texture1"), 0);

	// unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// game loop
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processKeyboardInput(window);

		// deltatime 
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// transformations
		glUseProgram(cubeShaderProgram);
		glBindVertexArray(cubeVao);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);

		// transformations in 3d
		view = glm::lookAt(cameraPosition, cameraPosition + cameraLookingAt, cameraUp);

		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// setting the lighting values
		glUniform3f(glGetUniformLocation(cubeShaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(cubeShaderProgram, "objectColor"), 1.0f, 0.5f, 0.31f);

		glUniform3i(glGetUniformLocation(cubeShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		// draw calls for cube
		for (int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		

		// draw calls for light
		glUseProgram(lightShaderProgram);
		glBindVertexArray(lightVao);

		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);

		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}