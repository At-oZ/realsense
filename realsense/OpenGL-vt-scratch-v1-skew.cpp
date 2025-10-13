/*
    OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
    v1:1つのカメラで1つの点を撮影する
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>

#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

using namespace std;

const unsigned int WIN_W = 800;
const unsigned int WIN_H = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main(void) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	Shader ourShader("shader-vt-v1.vert", "shader-vt-v1.frag");

	float vertices[] = {
		// positions         // colors
		19.950f, 19.950f, 20.0f,  1.0f, 1.0f, 1.0f
	   //-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	   //-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	   // 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f
	};

	// generate VBO and VAO
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// bind Vertex Array Object
	glBindVertexArray(VAO);

	// copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 skewMat = glm::mat4
	(
		1.0f, 0.0f, 0.9975f, 0.0f,
		0.0f, 1.0f, 0.9975f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	//glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIN_W / (float)WIN_H, 2.0f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,0.1f,0.0f));
		ourShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		ourShader.setFloat("pSize", 10.0f);
		ourShader.setMat4("skew", skewMat);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	vector<unsigned char> buf(WIN_W * WIN_H * 3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, WIN_W, WIN_H, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
	
	cv::Mat img(WIN_H, WIN_W, CV_8UC3);
	for (int y = 0; y < WIN_H; ++y) {
	    unsigned char* dst = img.ptr<unsigned char>(WIN_H - 1 - y);
	    const unsigned char* src = buf.data() + y * WIN_W * 3;
	    for (int x = 0; x < WIN_W; ++x) {
	        dst[x * 3 + 0] = src[x * 3 + 2]; // B
	        dst[x * 3 + 1] = src[x * 3 + 1]; // G
	        dst[x * 3 + 2] = src[x * 3 + 0]; // R
	    }
	}
	
	std::ostringstream stream;
	stream << "./images/scratch/sample-skew.png";
	std::string outPath = stream.str();

	cv::imwrite(outPath, img);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}