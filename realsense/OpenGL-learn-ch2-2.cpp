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
//	float vertices[] = {
//		 0.5f,  0.5f, 0.0f,  // top right
//		 0.5f, -0.5f, 0.0f,  // bottom right
//		-0.5f, -0.5f, 0.0f,  // bottom left
//		-0.5f,  0.5f, 0.0f   // top left 
//	};
//	unsigned int indices[] = {  // note that we start from 0!
//		0, 1, 3,   // first triangle
//		1, 2, 3    // second triangle
//	};
//
//	// generate VBO and VAO
//	unsigned int EBO, VBO, VAO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &EBO);
//	glGenBuffers(1, &VBO);
//
//	// bind Vertex Array Object
//	glBindVertexArray(VAO);
//
//	// copy our vertices array in a vertex buffer for OpenGL to use
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	// copy our index array in a element buffer for OpenGL to use
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// then set our vertex attributes pointers
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	// uncomment this call to draw in wireframe polygons.
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	while (!glfwWindowShouldClose(window))
//	{
//		processInput(window);
//
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		glUseProgram(shaderProgram);
//		glBindVertexArray(VAO);
//		//glDrawArrays(GL_TRIANGLES, 0, 6);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//	glDeleteProgram(shaderProgram);
//
//	glfwTerminate();
//	return 0;
//}