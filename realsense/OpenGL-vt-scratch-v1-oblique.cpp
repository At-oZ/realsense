///*
//	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
//	v1-oblique: 斜め(Oblique)プロジェクションを実装(v1からの派生)
//*/
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "shader_m.h"
//
//#include <opencv2/opencv.hpp>
//
//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//const unsigned int WIN_W = 800;
//const unsigned int WIN_H = 800;
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//
//int main(void) {
//
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Oblique Projection Example", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//	glEnable(GL_DEPTH_TEST);
//
//	// シェーダーは変更不要
//	Shader ourShader("shader-vt-v1.vert", "shader-vt-v1.frag");
//
//	float vertices[] = {
//		// positions         // colors
//		20.0f, 20.0f, 20.0f, 1.0f, 1.0f, 1.0f,
//		10.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f
//	};
//
//	int numVert = sizeof(vertices) / (sizeof(float) * 6);
//	cout << "Num vertices:" << numVert << endl;
//
//	unsigned int VBO, VAO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	// ----- 変更点 1: skewMat は不要なので削除 -----
//	// glm::mat4 skewMat = ...;
//
//	while (!glfwWindowShouldClose(window))
//	{
//		processInput(window);
//
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		ourShader.use();
//
//		// ----- 変更点 2: プロジェクション行列の生成方法を glm::frustum に変更 -----
//
//		// 1. 基本的なパラメータを設定
//		float fov = glm::radians(90.0f);
//		float aspectRatio = (float)WIN_W / (float)WIN_H;
//		float nearPlane = 2.0f;
//		float farPlane = 100.0f;
//
//		// 2. 対称なビューボリュームの境界を計算
//		float top = tan(fov / 2.0f) * nearPlane;
//		float bottom = -top;
//		float right = top * aspectRatio;
//		float left = -right;
//
//		// 3. 傾けたい角度からnearクリップ面でのズレ量を計算
//		float angleX = -20.0f / 20.0f;
//		float angleY = 20.0f / 20.0f;
//
//		float shiftX = nearPlane * angleX;
//		float shiftY = nearPlane * angleY;
//
//		// 4. 境界値にズレ量を加算
//		float finalLeft = left + shiftX;
//		float finalRight = right + shiftX;
//		float finalBottom = bottom + shiftY;
//		float finalTop = top + shiftY;
//
//		// 5. glm::frustumで斜めプロジェクション行列を生成
//		glm::mat4 projection = glm::frustum(finalLeft, finalRight, finalBottom, finalTop, nearPlane, farPlane);
//		ourShader.setMat4("projection", projection);
//
//		// --------------------------------------------------------------------
//
//		// camera/view transformation (変更不要)
//		// Note: カメラの上方向ベクトルは通常 (0,1,0) ですが、(0,0.1,0)でもlookAt内で正規化されるため動作します
//		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		ourShader.setMat4("view", view);
//
//		// model matrix (変更不要)
//		glm::mat4 model = glm::mat4(1.0f);
//		ourShader.setMat4("model", model);
//
//		ourShader.setFloat("pSize", 10.0f);
//
//		// ----- 変更点 3: skewMatの送信コードは不要なので削除 -----
//		// ourShader.setMat4("skew", skewMat);
//
//		glBindVertexArray(VAO);
//		glDrawArrays(GL_POINTS, 0, numVert); // 点は1つなので、個数を1に修正
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// (以降の画像保存処理などは変更なし)
//	vector<unsigned char> buf(WIN_W * WIN_H * 3);
//	glReadBuffer(GL_FRONT);
//	glReadPixels(0, 0, WIN_W, WIN_H, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
//
//	cv::Mat img(WIN_H, WIN_W, CV_8UC3);
//	for (int y = 0; y < WIN_H; ++y) {
//		unsigned char* dst = img.ptr<unsigned char>(WIN_H - 1 - y);
//		const unsigned char* src = buf.data() + y * WIN_W * 3;
//		for (int x = 0; x < WIN_W; ++x) {
//			dst[x * 3 + 0] = src[x * 3 + 2]; // B
//			dst[x * 3 + 1] = src[x * 3 + 1]; // G
//			dst[x * 3 + 2] = src[x * 3 + 0]; // R
//		}
//	}
//
//	std::ostringstream stream;
//	stream << "./sample-oblique.png"; // 出力ファイル名を変更
//	std::string outPath = stream.str();
//
//	cv::imwrite(outPath, img);
//
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//
//	glfwTerminate();
//	return 0;
//}
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
