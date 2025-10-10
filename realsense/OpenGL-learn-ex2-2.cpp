//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//using namespace std;
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//void main()
//{
//	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
//}
//)";
//
//const char* fragmentShaderSource = R"(
//#version 330 core
//out vec4 FragColor;
//
//void main()
//{
//	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//}
//)";
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
//
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//}
//
//int main(void) {
//
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	//glViewport(0, 0, 800, 600);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	// create vertex shader
//	unsigned int vertexShader;
//	vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glCompileShader(vertexShader);
//
//	// view compilation log
//	int success;
//	char infoLog[512];
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
//	}
//
//	// create fragment shader
//	unsigned int fragmentShader;
//	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//	glCompileShader(fragmentShader);
//
//	// view compilation log
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
//	}
//
//	// create program
//	unsigned int shaderProgram;
//	shaderProgram = glCreateProgram();
//
//	// link shaders to program
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	// view linking log
//	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
//		cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
//	}
//
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//	float vertices1[] = {
//		-0.5f, -0.25f, 0.0f,
//		 0.0f, -0.25f, 0.0f,
//		-0.25f,  0.25f, 0.0f
//	};
//	float vertices2[] = {
//		-0.0f, -0.25f, 0.0f,
//		 0.5f, -0.25f, 0.0f,
//		 0.25f,  0.25f, 0.0f
//	};
//
//	// generate VBO and VAO
//	unsigned int VBO1, VAO1;
//	glGenVertexArrays(1, &VAO1);
//	glGenBuffers(1, &VBO1);
//
//	// bind Vertex Array Object
//	glBindVertexArray(VAO1);
//
//	// copy our vertices array in a vertex buffer for OpenGL to use
//	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
//
//	// then set our vertex attributes pointers
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	//// generate VBO and VAO
//	//unsigned int VBO2, VAO2;
//	//glGenVertexArrays(1, &VAO2);
//	//glGenBuffers(1, &VBO2);
//
//	//// bind Vertex Array Object
//	//glBindVertexArray(VAO2);
//
//	//// copy our vertices array in a vertex buffer for OpenGL to use
//	//glBindBuffer(GL_ARRAY_BUFFER, VBO2);
//	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
//
//	//// then set our vertex attributes pointers
//	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	//glEnableVertexAttribArray(0);
//
//	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//	//glBindVertexArray(0);
//
//
//	// uncomment this call to draw in wireframe polygons.
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	while (!glfwWindowShouldClose(window))
//	{
//		processInput(window);
//
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		glUseProgram(shaderProgram);
//		glBindVertexArray(VAO1);
//		glDrawArrays(GL_TRIANGLES, 0, 4);
//		//glBindVertexArray(VAO2);
//		//glDrawArrays(GL_TRIANGLES, 0, 4);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glDeleteVertexArrays(1, &VAO1);
//	glDeleteBuffers(1, &VBO1);
//	//glDeleteVertexArrays(1, &VAO2);
//	//glDeleteBuffers(1, &VBO2);
//	glDeleteProgram(shaderProgram);
//
//	glfwTerminate();
//	return 0;
//}