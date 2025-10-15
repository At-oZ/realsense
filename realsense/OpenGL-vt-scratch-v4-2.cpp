///*
//	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
//	v4-2:処理速度の高速化(テクスチャ付き平面オブジェクト化)
//	注意点:各方向のレンズ数、点群数ともに偶数を仮定！
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
//#include <stb_image.h>
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
//const float LENS_ARRAY_W = 0.1804f;
//const float LENS_ARRAY_H = 0.1804f;
//const float LENS_PITCH_X = LENS_ARRAY_W / (float)NUM_LENS_W;
//const float LENS_PITCH_Y = LENS_ARRAY_H / (float)NUM_LENS_H;
//
//
//const float HALF_LENS_PITCH_X = LENS_PITCH_X * 0.5f;
//const float HALF_LENS_PITCH_Y = LENS_PITCH_Y * 0.5f;
//
//// 無限遠に向けた光線場再現の場合(B-1)
//const float ELEM_IMG_PITCH_X = LENS_PITCH_X;
//const float ELEM_IMG_PITCH_Y = LENS_PITCH_Y;
//
////// 想定観察距離に向けた光線場再現の場合(B-2)
////const float ELEM_IMG_PITCH_X = (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z * LENS_PITCH_X;
////const float ELEM_IMG_PITCH_Y = (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z * LENS_PITCH_Y;
//
//const float FLOAT_NUM_ELEM_IMG_PX_X = ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH;
//const float FLOAT_NUM_ELEM_IMG_PX_Y = ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH;
//const unsigned int NUM_ELEM_IMG_PX_X = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_X);
//const unsigned int NUM_ELEM_IMG_PX_Y = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_Y);
//
//const float DISPLAY_AREA_SIZE_X = NUM_ELEM_IMG_PX_X * NUM_LENS_W * DISPLAY_PX_PITCH;
//const float DISPLAY_AREA_SIZE_Y = NUM_ELEM_IMG_PX_Y * NUM_LENS_H * DISPLAY_PX_PITCH;
//
//// 仮想カメラ設定
//const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * (float)NUM_LENS_H - 1.0f);
//const float TAN_HALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
//const float FOV_Y = atan(TAN_HALF_Y) * 2.0f;
//const unsigned int WIN_W = NUM_LENS_W * NUM_ELEM_IMG_PX_X;
//const unsigned int WIN_H = NUM_LENS_H * NUM_ELEM_IMG_PX_Y;
//
////------------------------------
//
//
//// 定数(被写体側)
////------------------------------
//
//const float SUBJECT_Z = 1.0f;
//const unsigned int NUM_SUBJECT_POINTS_X = 300;
//const unsigned int NUM_SUBJECT_POINTS_Y = 300;
//const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;
//const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
//const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;
//const float SUBJECT_SIZE_X = DISPLAY_AREA_SIZE_X * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//const float SUBJECT_SIZE_Y = DISPLAY_AREA_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X - 1);
//const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y - 1);
//const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
//const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;
//
////------------------------------
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//static void printGLInfo(const char* tag);
//static GLFWmonitor* getMonitorByIndex(int index);
//
//int main(void) {
//
//	cout << "MIN OBSERVE Z:" << MIN_OBSERVE_Z << endl;
//	cout << "//----------------------------" << endl;
//	cout << "DISPLAY PIXEL PITCH:" << DISPLAY_PX_PITCH << endl;
//	cout << "NUM LENS WIDTH:" << NUM_LENS_W << endl;
//	cout << "NUM LENS HEIGHT:" << NUM_LENS_H << endl;
//	cout << "LENS ARRAY WIDTH:" << LENS_ARRAY_W << endl;
//	cout << "LENS ARRAY HEIGHT:" << LENS_ARRAY_H << endl;
//	cout << "LENS PITCH X:" << LENS_PITCH_X << endl;
//	cout << "LENS PITCH Y:" << LENS_PITCH_Y << endl;
//	cout << endl;
//	cout << "ELEMENTAL IMAGE PITCH X:" << ELEM_IMG_PITCH_X << endl;
//	cout << "ELEMENTAL IMAGE PITCH Y:" << ELEM_IMG_PITCH_Y << endl;
//	cout << "FLOAT NUM ELEMENTAL IMAGE PX X:" << FLOAT_NUM_ELEM_IMG_PX_X << endl;
//	cout << "FLOAT NUM ELEMENTAL IMAGE PX Y:" << FLOAT_NUM_ELEM_IMG_PX_Y << endl;
//	cout << "NUM ELEMENTAL IMAGE PX X:" << NUM_ELEM_IMG_PX_X << endl;
//	cout << "NUM ELEMENTAL IMAGE PX Y:" << NUM_ELEM_IMG_PX_Y << endl;
//	cout << "DISPLAY AREA SIZE X:" << DISPLAY_AREA_SIZE_X << endl;
//	cout << "DISPLAY AREA SIZE Y:" << DISPLAY_AREA_SIZE_Y << endl;
//	cout << endl;
//	cout << "FOCAL LENGTH:" << FOCAL_LENGTH << endl;
//	cout << "TAN HALF Y:" << TAN_HALF_Y << endl;
//	cout << "FOV Y:" << FOV_Y << endl;
//	cout << "WINDOW WIDTH:" << WIN_W << endl;
//	cout << "WINDOW HEIGHT:" << WIN_H << endl;
//	cout << "//----------------------------" << endl;
//	cout << "SUBJECT Z:" << SUBJECT_Z << endl;
//	cout << "NUM SUBJECT POINTS X:" << NUM_SUBJECT_POINTS_X << endl;
//	cout << "NUM SUBJECT POINTS Y:" << NUM_SUBJECT_POINTS_Y << endl;
//	cout << "NUM POINTS:" << NUM_POINTS << endl;
//	cout << "SUBJECT SIZE X:" << SUBJECT_SIZE_X << endl;
//	cout << "SUBJECT SIZE Y:" << SUBJECT_SIZE_Y << endl;
//	cout << "SUBJECT POINTS PITCH X:" << SUBJECT_POINTS_PITCH_X << endl;
//	cout << "SUBJECT POINTS PITCH Y:" << SUBJECT_POINTS_PITCH_Y << endl;
//	cout << "//----------------------------" << endl;
//	cout << endl;
//
//
//	// GLFWの初期設定
//	//------------------------------
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
//	//------------------------------
//
//
//	// OpenGLの各種機能
//	//------------------------------
//
//	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//
//	//------------------------------
//
//
//	// 頂点シェーダとフラグメントシェーダの指定
//	Shader ourShader("shader-vt-v4-2.vert", "shader-vt-v4-2.frag");
//
//
//	// 点群データの生成
//	//------------------------------
//	
//	//// 被写体画像読み込み
//	//cv::Mat image_input = cv::imread("./images/standard/grid_image.png");
//	//if (image_input.empty()) {
//	//	std::cout << "画像を開くことができませんでした。\n";
//	//	return -1;
//	//}
//	//cv::Mat resized_image;
//	//cv::resize(image_input, resized_image, cv::Size((int)NUM_SUBJECT_POINTS_Y, (int)NUM_SUBJECT_POINTS_X), 0, 0, cv::INTER_NEAREST);
//
//	//// 点群
//	//float* points = new float[NUM_POINTS * 6];
//	//for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
//	//{
//	//	int row = r + HALF_NUM_SUBJECT_POINTS_Y;
//	//	int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;
//
//	//	for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
//	//	{
//	//		int col = c + HALF_NUM_SUBJECT_POINTS_X;
//	//		int idx = (row * NUM_SUBJECT_POINTS_X + col) * 6;
//
//	//		points[idx] = (2.0f * (float)c + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X;
//	//		points[idx + 1] = (2.0f * (float)r + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y;
//	//		points[idx + 2] = SUBJECT_Z;
//	//		points[idx + 3] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[2] / 255.0f);
//	//		points[idx + 4] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[1] / 255.0f);
//	//		points[idx + 5] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[0] / 255.0f);
//
//	//	}
//	//}
//
//	//cout << "SUBJECT POINT OFFSET X:" << points[0] << endl;
//	//cout << "SUBJECT POINT OFFSET Y:" << points[1] << endl;
//
//	//------------------------------
//
//
//	// カメラ配置
//	glm::vec3* camPos = new glm::vec3[NUM_LENS_W * NUM_LENS_H];
//	for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r)
//	{
//		int row = r + HALF_NUM_LENS_H;
//		float camPosY = (2.0f * (float)r + 1.0f) * HALF_LENS_PITCH_Y;
//
//		for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c)
//		{
//			int col = c + HALF_NUM_LENS_W;
//			float camPosX = (2.0f * (float)c + 1.0f) * HALF_LENS_PITCH_X;
//
//			camPos[row * NUM_LENS_W + col] = glm::vec3(camPosX, camPosY, 0.0f);
//
//			//cout << "camPos[" << row * NUM_LENS_W + col << "]:(" << camPos[row * NUM_LENS_W + col].x << "," << camPos[row * NUM_LENS_W + col].y << "," << camPos[row * NUM_LENS_W + col].z << ")" << endl;
//		}
//	}
//
//	float vertices[] = {
//		// positions          // colors           // texture coords
//		(2.0f * (float)HALF_NUM_SUBJECT_POINTS_X + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X, (2.0f * (float)HALF_NUM_SUBJECT_POINTS_Y + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y, SUBJECT_Z,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
//		(2.0f * (float)HALF_NUM_SUBJECT_POINTS_X + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X, (2.0f * -(float)HALF_NUM_SUBJECT_POINTS_Y + 1.0f)* HALF_SUBJECT_POINTS_PITCH_Y, SUBJECT_Z,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
//		(2.0f * -(float)HALF_NUM_SUBJECT_POINTS_X + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X,(2.0f * -(float)HALF_NUM_SUBJECT_POINTS_Y + 1.0f)* HALF_SUBJECT_POINTS_PITCH_Y, SUBJECT_Z,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
//		(2.0f * -(float)HALF_NUM_SUBJECT_POINTS_X + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X,(2.0f * (float)HALF_NUM_SUBJECT_POINTS_Y + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y, SUBJECT_Z,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
//	};
//	unsigned int indices[] = {
//		0, 1, 3, // first triangle
//		1, 2, 3  // second triangle
//	};
//
//	// VBOとVAOとEBOの設定
//	//------------------------------
//
//	unsigned int VBO, VAO, EBO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// color attribute
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//	// texture coord attribute
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glEnableVertexAttribArray(2);
//
//	//------------------------------
//
//
//	// テクスチャのロードと作成
//	//------------------------------
//	unsigned int texture1;
//	// texture 1
//	// ---------
//	glGenTextures(1, &texture1);
//	glBindTexture(GL_TEXTURE_2D, texture1);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//	unsigned char* data = stbi_load("./images/standard/grid_image.png", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//	ourShader.use();
//	ourShader.setInt("texture1", 0);
//
//	//------------------------------
//
//
//	// フレーム処理
//	//------------------------------
//
//	glfwSetFramebufferSizeCallback(gridWin, framebuffer_size_callback);
//
//	while (!glfwWindowShouldClose(gridWin))
//	{
//		processInput(gridWin);
//
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		//ourShader.setFloat("pSize", 0.1f);
//
//		// bind textures on corresponding texture units
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, texture1);
//
//		ourShader.use();
//		glBindVertexArray(VAO);
//
//		// pass projection matrix to shader (note that in this case it could change every frame)
//		glm::mat4 projection = glm::perspective(FOV_Y, (float)NUM_ELEM_IMG_PX_X / (float)NUM_ELEM_IMG_PX_Y, 0.1f, 100.0f);
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
//				int idx = row * NUM_LENS_W + col;
//
//				int vx = (NUM_LENS_W - 1 - col) * NUM_ELEM_IMG_PX_X;
//				int vy = row * NUM_ELEM_IMG_PX_Y;
//
//				glViewport(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//				glScissor(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//
//				// camera/view transformation
//				glm::mat4 view = glm::lookAt(camPos[idx], glm::vec3(camPos[idx].x, camPos[idx].y, 1.0f), glm::vec3(0.0f, 0.1f, 0.0f));
//				ourShader.setMat4("view", view);
//
//				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
//	//------------------------------
//
//
//	// 表示画像の保存
//	//------------------------------
//
//	//vector<unsigned char> buf(WIN_W * WIN_H * 3);
//	//glReadBuffer(GL_FRONT);
//	//glReadPixels(0, 0, WIN_W, WIN_H, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
//
//	//cv::Mat img(WIN_H, WIN_W, CV_8UC3);
//	//for (int y = 0; y < WIN_H; ++y) {
//	//	unsigned char* dst = img.ptr<unsigned char>(WIN_H - 1 - y);
//	//	const unsigned char* src = buf.data() + y * WIN_W * 3;
//	//	for (int x = 0; x < WIN_W; ++x) {
//	//		dst[x * 3 + 0] = src[(WIN_W - 1 - x) * 3 + 2]; // B
//	//		dst[x * 3 + 1] = src[(WIN_W - 1 - x) * 3 + 1]; // G
//	//		dst[x * 3 + 2] = src[(WIN_W - 1 - x) * 3 + 0]; // R
//	//	}
//	//}
//
//	//std::ostringstream stream;
//	//stream << "D:/ForStudy/reconstruction/OpenGL-scratch-normal-v1/OpenGL-scratch-normal-v1-grid_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << ".png";
//	//std::string outPath = stream.str();
//
//	//cv::imwrite(outPath, img);
//
//	//------------------------------
//
//
//	// 後処理
//	//------------------------------
//
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//
//	delete[] camPos;
//	//delete[] points;
//
//	glfwTerminate();
//
//	//------------------------------
//
//
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
