/*
	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
	v2-oblique:斜め(oblique)プロジェクションを実装(v2からの派生)
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

const unsigned int VIEW_W = 400;
const unsigned int VIEW_H = 400;

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
	glEnable(GL_DEPTH_TEST);

	Shader ourShader("shader-vt-v1.vert", "shader-vt-v1.frag");

	float vertices[] = {
		// positions         // colors
		-10.0f, 10.0f, 10.0f,  1.0f, 1.0f, 1.0f,
		10.0f, 10.0f, 10.0f,  1.0f, 0.0f, 0.0f,
		-10.0f, -10.0f, 10.0f,  0.0f, 1.0f, 0.0f,
		10.0f, -10.0f, 10.0f,  0.0f, 0.0f, 1.0f
	};

	int numVert = sizeof(vertices) / sizeof(float) / 6;

	float angles[] = {
		-10.0f, -10.0f, 10.0f,
		10.0f, -10.0f, 10.0f,
		-10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f
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

	// 1. 基本的なパラメータを設定
	float fov = glm::radians(90.0f);
	float aspectRatio = (float)WIN_W / (float)WIN_H;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	// 2. 対称なビューボリュームの境界を計算
	float top = tan(fov / 2.0f) * nearPlane;
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//ourShader.setFloat("pSize", 0.1f);

		ourShader.use();
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		ourShader.setMat4("model", model);

		ourShader.setFloat("pSize", 10.0f);

		glEnable(GL_SCISSOR_TEST);
        for (int r = -1; r < 1; ++r) {
			for (int c = -1; c < 1; ++c) {

				int idx = ((r + 1) * 2 + (c + 1)) * 3;
				float invZ = 1.0f / angles[idx + 2];

				glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);

				// 3. 傾けたい角度からnearクリップ面でのズレ量を計算
				float angleX = (angles[idx] - camPos.x) * invZ;
				float angleY = (angles[idx + 1] - camPos.y) * invZ;

				float shiftX = nearPlane * angleX;
				float shiftY = nearPlane * angleY;

				// 4. 境界値にズレ量を加算
				float finalLeft = left + shiftX;
				float finalRight = right + shiftX;
				float finalBottom = bottom + shiftY;
				float finalTop = top + shiftY;

                int vx = (c + 1) * VIEW_W;
                int vy = (r + 1) * VIEW_H;

                glViewport(vx, vy, VIEW_W, VIEW_H);
                glScissor(vx, vy, VIEW_W, VIEW_H);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// pass projection matrix to shader (note that in this case it could change every frame)
				glm::mat4 projection = glm::frustum(finalLeft, finalRight, finalBottom, finalTop, nearPlane, farPlane);
				//glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIN_W / (float)WIN_H, 0.1f, 100.0f);
				ourShader.setMat4("projection", projection);

				// camera/view transformation
				glm::mat4 view = glm::lookAt(camPos, glm::vec3(camPos.x, camPos.y, 1.0f), glm::vec3(0.0f, 0.1f, 0.0f));
				ourShader.setMat4("view", view);

				glDrawArrays(GL_POINTS, 0, numVert);
            }
        }
		glDisable(GL_SCISSOR_TEST);
		glBindVertexArray(0);
		glUseProgram(0);


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
	stream << "./images/scratch/sample-oblique-v2.png";
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
