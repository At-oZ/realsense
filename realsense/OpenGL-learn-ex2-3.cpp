#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#define GLFW_FALSE 0
#endif

using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const char* fragmentShaderSource1 = R"(
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
}
)";

const char* fragmentShaderSource2 = R"(
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
)";


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(void) {

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
	//glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// view compilation log
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// create fragment shader 1
	unsigned int fragmentShader[2];
	fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader[0], 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader[0]);

	// view compilation log
	glGetShaderiv(fragmentShader[0], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader[0], 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// create program 1
	unsigned int shaderProgram[2];
	shaderProgram[0] = glCreateProgram();

	// link shaders to program
	glAttachShader(shaderProgram[0], vertexShader);
	glAttachShader(shaderProgram[0], fragmentShader[0]);
	glLinkProgram(shaderProgram[0]);

	// view linking log
	glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram[0], 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(fragmentShader[0]);

	// create fragment shader 2
	fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader[1], 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader[1]);

	// view compilation log
	glGetShaderiv(fragmentShader[1], GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader[1], 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// create program 1
	shaderProgram[1] = glCreateProgram();

	// link shaders to program
	glAttachShader(shaderProgram[1], vertexShader);
	glAttachShader(shaderProgram[1], fragmentShader[1]);
	glLinkProgram(shaderProgram[1]);

	// view linking log
	glGetProgramiv(shaderProgram[1], GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram[1], 512, NULL, infoLog);
		cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader[1]);


	float vertices1[] = {
		-0.5f, -0.25f, 0.0f,
		 0.0f, -0.25f, 0.0f,
		-0.25f,  0.25f, 0.0f
	};
	float vertices2[] = {
		-0.0f, -0.25f, 0.0f,
		 0.5f, -0.25f, 0.0f,
		 0.25f,  0.25f, 0.0f
	};

	unsigned int VBO[2], VAO[2];
	glGenVertexArrays(2, &VAO[0]);
	glGenBuffers(2, &VBO[0]);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram[0]);
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 4);

		glUseProgram(shaderProgram[1]);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO[0]);
	glDeleteBuffers(1, &VBO[0]);
	glDeleteVertexArrays(1, &VAO[1]);
	glDeleteBuffers(1, &VBO[1]);
	glDeleteProgram(shaderProgram[0]);
	glDeleteProgram(shaderProgram[1]);

	glfwTerminate();
	return 0;
}