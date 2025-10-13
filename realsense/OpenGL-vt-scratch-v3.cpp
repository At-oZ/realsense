///*
//	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
//	v3:N×Nのカメラで1つの点を撮影する
//*/
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <shader_m.h>
//
//#include <opencv2/opencv.hpp>
//
//#include <iostream>
//#include <vector>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//// dGPU 要求 (Optimus / PowerXpress)
//extern "C" {
//	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}
//
//using namespace std;
//
//// 定数(観察者側)
////------------------------------
//
//const float MIN_OBSERVE_Z = 1.0f;
//
////------------------------------
//
//
//// 定数(表示系側)
////------------------------------
//
//const float DISPLAY_PX_PITCH = 13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);
//
//const unsigned int NUM_LENS_W = 40;
//const unsigned int NUM_LENS_H = 40;
//
//const int HALF_NUM_LENS_W = NUM_LENS_W * 0.5f;
//const int HALF_NUM_LENS_H = NUM_LENS_H * 0.5f;
//
////// レンズピッチを固定する場合(A-1)
////const float LENS_PITCH_X = 0.00451f;
////const float LENS_PITCH_Y = 0.00451f;
////const float DISPLAY_AREA_SIZE_X = LENS_PITCH_X * NUM_LENS_W;
////const float DISPLAY_AREA_SIZE_Y = LENS_PITCH_Y * NUM_LENS_H;
//
//// レンズアレイ幅を固定する場合(A-2)
//const float LENS_ARRAY_WIDTH = 0.1804f;
//const float LENS_ARRAY_HEIGHT = 0.1804f;
//const float LENS_PITCH_X = LENS_ARRAY_WIDTH / (float)NUM_LENS_W;
//const float LENS_PITCH_Y = LENS_ARRAY_HEIGHT / (float)NUM_LENS_H;
//const float DISPLAY_AREA_SIZE_X = LENS_ARRAY_WIDTH;
//const float DISPLAY_AREA_SIZE_Y = LENS_ARRAY_HEIGHT;
//
//const float HALF_LENS_PITCH_X = LENS_PITCH_X * 0.5f;
//const float HALF_LENS_PITCH_Y = LENS_PITCH_Y * 0.5f;
//
//// 無限遠に向けた光線場再現の場合(B-1)
//const unsigned int NUM_ELEM_IMG_PX_X = static_cast<unsigned int>(LENS_PITCH_X / DISPLAY_PX_PITCH);
//const unsigned int NUM_ELEM_IMG_PX_Y = static_cast<unsigned int>(LENS_PITCH_Y / DISPLAY_PX_PITCH);
//
////// 想定観察距離に向けた光線場再現の場合(B-2)
////const float ELEM_IMG_PITCH_X = (FOCAL_LENGTH + MIN_Z_OBSERVE) / MIN_Z_OBSERVE * LENS_PITCH_X;
////const float ELEM_IMG_PITCH_Y = (FOCAL_LENGTH + MIN_Z_OBSERVE) / MIN_Z_OBSERVE * LENS_PITCH_Y;
////const unsigned int NUM_ELEM_IMG_PX_X = static_cast<unsigned int>(ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH);
////const unsigned int NUM_ELEM_IMG_PX_Y = static_cast<unsigned int>(ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH);
//
//
//// 仮想カメラ設定
//const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * (float)NUM_LENS_H - 1.0f);
//const float TAN_HALF_Y = DISPLAY_PX_PITCH * NUM_ELEM_IMG_PX_Y / FOCAL_LENGTH * 0.5f;
//const float FOV_Y = atan(TAN_HALF_Y) * 2.0f;
//const unsigned int WIN_W = NUM_LENS_W * NUM_ELEM_IMG_PX_X;
//const unsigned int WIN_H = NUM_LENS_H * NUM_ELEM_IMG_PX_Y;
//
////------------------------------
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//static void printGLInfo(const char* tag);
//static GLFWmonitor* getMonitorByIndex(int index);
//
//
//int main(void) {
//
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	int targetMonitor = 0;
//	GLFWmonitor* mon = getMonitorByIndex(targetMonitor);
//	if (!mon) mon = glfwGetPrimaryMonitor();
//	const GLFWvidmode* mode = glfwGetVideoMode(mon);
//
//	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
//#ifdef GLFW_FLOATING
//	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
//#endif
//	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//
//	int offsetX = 0;
//	int offsetY = 0;
//	int winW = max((int)WIN_W, mode->width - offsetX);
//	int winH = max((int)WIN_H, mode->height - offsetY);
//
//	GLFWwindow* gridWin = glfwCreateWindow(winW, winH, "Grid Cameras", nullptr, nullptr);
//	if (!gridWin) { std::fprintf(stderr, "Create window failed\n"); glfwTerminate(); return -1; }
//	glfwMakeContextCurrent(gridWin);
//	glfwSwapInterval(1);
//
//	int mx = 0, my = 0;
//	glfwGetMonitorPos(mon, &mx, &my);
//	glfwSetWindowPos(gridWin, mx + offsetX, my + offsetY);
//#ifdef GLFW_FLOATING
//	glfwSetWindowAttrib(gridWin, GLFW_FLOATING, GLFW_TRUE);
//#endif
//	glfwShowWindow(gridWin);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::fprintf(stderr, "GLAD init failed\n"); return -1;
//	}
//	printGLInfo("Grid Cameras");
//
//	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//
//	Shader ourShader("shader-vt-v1.vert", "shader-vt-v1.frag");
//
//	float vertices[] = {
//		// positions         // colors
//		0.0f, 0.0f, 20.0f,  1.0f, 1.0f, 1.0f
//		//-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
//		//-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
//		// 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f
//	};
//
//	cout << "sizeof(vertices):" << sizeof(vertices) << endl;
//
//	glm::vec3* camPos = new glm::vec3[NUM_LENS_W * NUM_LENS_H];
//	for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r)
//	{
//		int row = r + HALF_NUM_LENS_H;
//
//		for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c)
//		{
//			int col = c + HALF_NUM_LENS_W;
//			camPos[row * NUM_LENS_W + col] = glm::vec3((2.0f * (float)c + 1.0f) * HALF_LENS_PITCH_X, (2.0f * (float)r + 1.0f) * HALF_LENS_PITCH_Y, 0.0f);
//			//cout << "camPos[" << row * NUM_LENS_W + col << "]:(" << camPos[row * NUM_LENS_W + col].x << "," << camPos[row * NUM_LENS_W + col].y << "," << camPos[row * NUM_LENS_W + col].z << ")" << endl;
//		}
//	}
//
//	// generate VBO and VAO
//	unsigned int VBO, VAO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//
//	// bind Vertex Array Object
//	glBindVertexArray(VAO);
//
//	// copy our vertices array in a buffer for OpenGL to use
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// color attribute
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	//glViewport(0, 0, 800, 600);
//	glfwSetFramebufferSizeCallback(gridWin, framebuffer_size_callback);
//
//	while (!glfwWindowShouldClose(gridWin))
//	{
//		processInput(gridWin);
//
//		glClearColor(0.0f, 0.2f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		//ourShader.setFloat("pSize", 0.1f);
//
//		ourShader.use();
//		glBindVertexArray(VAO);
//
//		// pass projection matrix to shader (note that in this case it could change every frame)
//		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIN_W / (float)WIN_H, 2.0f, 100.0f);
//		ourShader.setMat4("projection", projection);
//
//		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//		ourShader.setMat4("model", model);
//
//		glEnable(GL_SCISSOR_TEST);
//		for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r)
//		{
//		
//			int row = r + HALF_NUM_LENS_H;
//			for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c)
//			{
//
//				int col = c + HALF_NUM_LENS_W;
//
//				int vx = col * NUM_ELEM_IMG_PX_X;
//				int vy = row * NUM_ELEM_IMG_PX_Y;
//
//				glViewport(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//				glScissor(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//				glClear(GL_COLOR_BUFFER_BIT);
//
//				// camera/view transformation
//				glm::mat4 view = glm::lookAt(camPos[row * NUM_LENS_W + col], glm::vec3(camPos[row * NUM_LENS_W + col].x, camPos[row * NUM_LENS_W + col].y, 1.0f), glm::vec3(0.0f, 0.1f, 0.0f));
//				ourShader.setMat4("view", view);
//
//				glDrawArrays(GL_POINTS, 0, 1);
//			}
//		}
//		glDisable(GL_SCISSOR_TEST);
//		glBindVertexArray(0);
//		glUseProgram(0);
//
//
//		glfwSwapBuffers(gridWin);
//		glfwPollEvents();
//	}
//
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
//	stream << "./images/scratch/sample-1.png";
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
//
//static void printGLInfo(const char* tag) {
//	const GLubyte* vendor = glGetString(GL_VENDOR);
//	const GLubyte* renderer = glGetString(GL_RENDERER);
//	const GLubyte* version = glGetString(GL_VERSION);
//	const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
//	std::printf("[%s] GL_VENDOR   = %s\n", tag, vendor ? (const char*)vendor : "(null)");
//	std::printf("[%s] GL_RENDERER = %s\n", tag, renderer ? (const char*)renderer : "(null)");
//	std::printf("[%s] GL_VERSION  = %s\n", tag, version ? (const char*)version : "(null)");
//	std::printf("[%s] GLSL        = %s\n", tag, glsl ? (const char*)glsl : "(null)");
//}
//
//static GLFWmonitor* getMonitorByIndex(int index) {
//	int count = 0;
//	GLFWmonitor** mons = glfwGetMonitors(&count);
//	if (count <= 0) return nullptr;
//	if (index < 0 || index >= count) return glfwGetPrimaryMonitor();
//	return mons[index];
//}
