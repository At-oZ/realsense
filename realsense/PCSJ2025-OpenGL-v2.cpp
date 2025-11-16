///*
//	PCSJ 2025向けコード(OpenGL導入バージョン)
//	v1:OpenGL-vt-scratch-v4-2-3からの派生
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
//#include <thread>
//#include <mutex>
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
//// ディスプレイの画素ピッチ
//const float DISPLAY_PX_PITCH = 13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);
//
//// レンズ数
//const unsigned int NUM_LENS_X = 128;
//const unsigned int NUM_LENS_Y = 80;
//
//// レンズ数の半分
//const int HALF_NUM_LENS_X = NUM_LENS_X * 0.5f;
//const int HALF_NUM_LENS_Y = NUM_LENS_Y * 0.5f;
//
//// レンズの焦点距離
//const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * 40.0f - 1.0f);
//
///* 要素画像の解像度至上主義 */
//// 要素画像の解像度
//const unsigned int NUM_ELEM_IMG_PX_X = 60;
//const unsigned int NUM_ELEM_IMG_PX_Y = 60;
//
//// 要素画像の解像度の浮動小数点数
//const float FLOAT_NUM_ELEM_IMG_PX_X = (float)NUM_ELEM_IMG_PX_X;
//const float FLOAT_NUM_ELEM_IMG_PX_Y = (float)NUM_ELEM_IMG_PX_Y;
//
//// 要素画像の間隔
//const float ELEM_IMG_PITCH_X = NUM_ELEM_IMG_PX_X * DISPLAY_PX_PITCH;
//const float ELEM_IMG_PITCH_Y = NUM_ELEM_IMG_PX_Y * DISPLAY_PX_PITCH;
//
///* レンズピッチ */
////// 無限遠に向けた光線場再現の場合(A-1)
////const string VIEW_MODE = "normal";
////const float LENS_PITCH_X = ELEM_IMG_PITCH_X;
////const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y;
//
//// 想定観察距離に向けた光線場再現の場合(A-2)
//const string VIEW_MODE = "wideview";
//const float LENS_PITCH_X = ELEM_IMG_PITCH_X * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);
//const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);
//
//// レンズピッチの半分
//const float HALF_LENS_PITCH_X = LENS_PITCH_X / 2.0f;
//const float HALF_LENS_PITCH_Y = LENS_PITCH_Y / 2.0f;
//
//// 表示画像の解像度
//const unsigned int NUM_DISPLAY_IMG_PX_X = NUM_ELEM_IMG_PX_X * NUM_LENS_X;
//const unsigned int NUM_DISPLAY_IMG_PX_Y = NUM_ELEM_IMG_PX_Y * NUM_LENS_Y;
//
//// 表示画像のサイズ
//const float DISPLAY_IMG_SIZE_X = NUM_DISPLAY_IMG_PX_X * DISPLAY_PX_PITCH;
//const float DISPLAY_IMG_SIZE_Y = NUM_DISPLAY_IMG_PX_Y * DISPLAY_PX_PITCH;
//
//// 仮想カメラ設定
//const float TAN_YALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
//const float FOV_Y = atan(TAN_YALF_Y) * 2.0f;
//
////------------------------------
//
//
//// 定数(被写体側)
////------------------------------
//
//// 被写体平面の奥行位置
//const float SUBJECT_Z = 1.0f;
//
//// 被写体平面の各軸点群数
//const unsigned int NUM_SUBJECT_POINTS_X = 554;
//const unsigned int NUM_SUBJECT_POINTS_Y = 554;
//
//// 被写体平面の全体点群数
//const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;
//
//// 被写体平面の各軸点群数の半分
//const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
//const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;
//
//// 被写体平面のサイズ
//const float SUBJECT_SIZE_X = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//const float SUBJECT_SIZE_Y = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//
//// 被写体平面の点群間隔
//const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X);
//const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y);
//
//// 被写体平面の点群間隔の半分
//const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
//const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;
//
////------------------------------
//
//
//// 定数(3次元配列側)
////------------------------------
//
//const int BOX_DETAIL_N = 3;
//const float Z_PLANE_IMG_PITCH = DISPLAY_PX_PITCH / (float)BOX_DETAIL_N;
//const int Z_PLANE_IMG_PX_X = 960;
//const int Z_PLANE_IMG_PX_Y = 600;
//const int NUM_Z_PLANE = 60;
//const float BOX_MIN_Z = 0.2f;
//const float COEF_TRANS = ((float)NUM_Z_PLANE - 0.0f) * BOX_MIN_Z;
//const float INV_COEF_TRANS = 1.0f / COEF_TRANS;
//const int HALF_SEARCH_BOX_SIZE = (int)floor(BOX_DETAIL_N / 2);
//const float F_OVER_Z_PLANE_PITCH = FOCAL_LENGTH / Z_PLANE_IMG_PITCH;
//const int HALF_Z_PLANE_IMG_PX_X = Z_PLANE_IMG_PX_X / 2;
//const int HALF_Z_PLANE_IMG_PX_Y = Z_PLANE_IMG_PX_Y / 2;
//
//
////------------------------------
//
//
//// 定数(そのほか)
////------------------------------
//
//const int NUM_THREADS = 30;
//
////------------------------------
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//static void printGLInfo(const char* tag);
//static GLFWmonitor* getMonitorByIndex(int index);
//void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
//
//// CPU側でRGBAを層範囲[start,end)にパックするだけ（GLは呼ばない）
//void packSlicesRGBA(int start, int end,
//	const unsigned int* red, const unsigned int* green, const unsigned int* blue,
//	const unsigned int* numPoints, unsigned char* rgba3D);
//
//void averageSlices(int start, int end,
//	unsigned int* red, unsigned int* green, unsigned int* blue,
//	const unsigned int* numPoints);
//
//// start/end は pointsPos/pointsCol の「float×3/BGR×3の線形配列」での範囲（3刻み）
//void binningPointClouds(int start, int end,
//	const float* pointsPos, const unsigned char* pointsCol,
//	unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
//
//int main(void) {
//
//	std::cout << "MIN OBSERVE Z:" << MIN_OBSERVE_Z << std::endl;
//	std::cout << "//----------------------------" << std::endl;
//	std::cout << "DISPLAY PIXEL PITCH:" << DISPLAY_PX_PITCH << std::endl;
//	std::cout << "NUM LENS WIDTH:" << NUM_LENS_X << std::endl;
//	std::cout << "NUM LENS HEIGHT:" << NUM_LENS_Y << std::endl;
//	std::cout << "LENS PITCH X:" << LENS_PITCH_X << std::endl;
//	std::cout << "LENS PITCH Y:" << LENS_PITCH_Y << std::endl;
//	std::cout << std::endl;
//	std::cout << "ELEMENTAL IMAGE PITCH X:" << ELEM_IMG_PITCH_X << std::endl;
//	std::cout << "ELEMENTAL IMAGE PITCH Y:" << ELEM_IMG_PITCH_Y << std::endl;
//	std::cout << "FLOAT NUM ELEMENTAL IMAGE PX X:" << FLOAT_NUM_ELEM_IMG_PX_X << std::endl;
//	std::cout << "FLOAT NUM ELEMENTAL IMAGE PX Y:" << FLOAT_NUM_ELEM_IMG_PX_Y << std::endl;
//	std::cout << "NUM ELEMENTAL IMAGE PX X:" << NUM_ELEM_IMG_PX_X << std::endl;
//	std::cout << "NUM ELEMENTAL IMAGE PX Y:" << NUM_ELEM_IMG_PX_Y << std::endl;
//	std::cout << "DISPLAY IMG SIZE X:" << DISPLAY_IMG_SIZE_X << std::endl;
//	std::cout << "DISPLAY IMG SIZE Y:" << DISPLAY_IMG_SIZE_Y << std::endl;
//	std::cout << std::endl;
//	std::cout << "FOCAL LENGTH:" << FOCAL_LENGTH << std::endl;
//	std::cout << "TAN HALF Y:" << TAN_YALF_Y << std::endl;
//	std::cout << "FOV Y:" << FOV_Y << std::endl;
//	std::cout << "WINDOW WIDTH:" << NUM_DISPLAY_IMG_PX_X << std::endl;
//	std::cout << "WINDOW HEIGHT:" << NUM_DISPLAY_IMG_PX_Y << std::endl;
//	std::cout << "//----------------------------" << std::endl;
//	std::cout << "SUBJECT Z:" << SUBJECT_Z << std::endl;
//	std::cout << "NUM SUBJECT POINTS X:" << NUM_SUBJECT_POINTS_X << std::endl;
//	std::cout << "NUM SUBJECT POINTS Y:" << NUM_SUBJECT_POINTS_Y << std::endl;
//	std::cout << "NUM POINTS:" << NUM_POINTS << std::endl;
//	std::cout << "SUBJECT SIZE X:" << SUBJECT_SIZE_X << std::endl;
//	std::cout << "SUBJECT SIZE Y:" << SUBJECT_SIZE_Y << std::endl;
//	std::cout << "SUBJECT POINTS PITCH X:" << SUBJECT_POINTS_PITCH_X << std::endl;
//	std::cout << "SUBJECT POINTS PITCH Y:" << SUBJECT_POINTS_PITCH_Y << std::endl;
//	std::cout << "//----------------------------" << std::endl;
//	std::cout << std::endl;
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
//	int winW = max((int)NUM_DISPLAY_IMG_PX_X, mode->width - offsetX);
//	int winH = max((int)NUM_DISPLAY_IMG_PX_Y, mode->height - offsetY);
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
//	Shader compose("shader-slices-compose.vert", "shader-slices-compose.frag");
//	GLint locInvZ = glGetUniformLocation(compose.ID, "uInvZ");
//
//	// 点群データの生成
//	//------------------------------
//
//	// 被写体画像読み込み
//	cv::Mat image_input = cv::imread("./images/standard/pepper.bmp");
//	if (image_input.empty()) {
//		std::cout << "画像を開くことができませんでした。\n";
//		return -1;
//	}
//	cv::Mat resized_image;
//	cv::resize(image_input, resized_image, cv::Size((int)NUM_SUBJECT_POINTS_Y, (int)NUM_SUBJECT_POINTS_X), 0, 0, cv::INTER_NEAREST);
//
//	// 点群座標
//	float* pointsPos = new float[NUM_POINTS * 3];
//	for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
//	{
//		int row = r + HALF_NUM_SUBJECT_POINTS_Y;
//		int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;
//
//		for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
//		{
//			int col = c + HALF_NUM_SUBJECT_POINTS_X;
//			int idx = (row * NUM_SUBJECT_POINTS_X + col) * 3;
//
//			pointsPos[idx] = (2.0f * (float)c + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X;
//			pointsPos[idx + 1] = (2.0f * (float)r + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y;
//			pointsPos[idx + 2] = SUBJECT_Z;
//
//		}
//	}
//
//	// 点群色情報
//	unsigned char* pointsCol = new unsigned char[NUM_POINTS * 3];
//	for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
//	{
//		int row = r + HALF_NUM_SUBJECT_POINTS_Y;
//		int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;
//
//		for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
//		{
//			int col = c + HALF_NUM_SUBJECT_POINTS_X;
//			int idx = (row * NUM_SUBJECT_POINTS_X + col) * 3;
//
//			pointsCol[idx] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[2];
//			pointsCol[idx + 1] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[1];
//			pointsCol[idx + 2] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[0];
//
//		}
//	}
//
//	//std::cout << "SUBJECT POINT OFFSET X:" << points[0] << std::endl;
//	//std::cout << "SUBJECT POINT OFFSET Y:" << points[1] << std::endl;
//
//	//------------------------------
//
//
//	// カメラ配置
//	glm::vec3* camPos = new glm::vec3[NUM_LENS_X * NUM_LENS_Y];
//	for (int r = -HALF_NUM_LENS_Y; r < HALF_NUM_LENS_Y; ++r)
//	{
//		int row = r + HALF_NUM_LENS_Y;
//		float camPosY = (2.0f * (float)r + 1.0f) * HALF_LENS_PITCH_Y;
//
//		for (int c = -HALF_NUM_LENS_X; c < HALF_NUM_LENS_X; ++c)
//		{
//			int col = c + HALF_NUM_LENS_X;
//			float camPosX = (2.0f * (float)c + 1.0f) * HALF_LENS_PITCH_X;
//
//			camPos[row * NUM_LENS_X + col] = glm::vec3(camPosX, camPosY, 0.0f);
//
//			//std::cout << "camPos[" << row * NUM_LENS_X + col << "]:(" << camPos[row * NUM_LENS_X + col].x << "," << camPos[row * NUM_LENS_X + col].y << "," << camPos[row * NUM_LENS_X + col].z << ")" << std::endl;
//		}
//	}
//
//
//	// テクスチャ配列の作成
//	//------------------------------
//
//	GLuint texSlices = 0; // 60層のテクスチャ配列
//	{
//		// テクスチャ配列の作成
//		glGenTextures(1, &texSlices);
//		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
//		// 内部フォーマットを GL_RGBA8 に変更（アップロードはGL_UNSIGNED_BYTEのまま）
//		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//
//		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
//	}
//
//	// CPU→GPU 転送の一時バッファ（全層ぶんのRGBA）
//	std::vector<unsigned char> rgba3D(Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE * 4);
//
//	//------------------------------
//
//
//	// VBO, VAOの作成
//	//------------------------------
//
//	GLuint quadVAO = 0, quadVBO = 0;
//	{
//		const float quad[] = {
//			// pos    // uv
//			-1.f, -1.f, 0.f, 0.f,
//			 1.f, -1.f, 1.f, 0.f,
//			 1.f,  1.f, 1.f, 1.f,
//			-1.f, -1.f, 0.f, 0.f,
//			 1.f,  1.f, 1.f, 1.f,
//			-1.f,  1.f, 0.f, 1.f,
//		};
//		glGenVertexArrays(1, &quadVAO);
//		glGenBuffers(1, &quadVBO);
//		glBindVertexArray(quadVAO);
//		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindVertexArray(0);
//	}
//
//	//------------------------------
//
//
//	unsigned int* red = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* green = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* blue = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* numPoints = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//
//	const int NZ = NUM_Z_PLANE;
//	std::vector<float> invZ(NZ);
//	for (int n = 0; n < NZ; ++n) {
//		// z_t = 1/z を COEF_TRANS で量子化: bin中心を (n+0.5)/COEF_TRANS と近似
//		invZ[n] = ((float)n) * INV_COEF_TRANS; // = (n+0.5)/COEF_TRANS
//	}
//
//	// baseOffset の前計算部分を置換
//	const float texW = static_cast<float>(Z_PLANE_IMG_PX_X);
//	const float texH = static_cast<float>(Z_PLANE_IMG_PX_Y);
//	const float scaleX = FOCAL_LENGTH / Z_PLANE_IMG_PITCH / texW; // = (F/img_pitch)/TexW
//	const float scaleY = FOCAL_LENGTH / Z_PLANE_IMG_PITCH / texH; // = (F/img_pitch)/TexH
//
//	std::vector<glm::vec2> baseOffset(NUM_LENS_X * NUM_LENS_Y);
//	for (int r = -HALF_NUM_LENS_Y; r < HALF_NUM_LENS_Y; ++r) {
//		int row = r + HALF_NUM_LENS_Y;
//		for (int c = -HALF_NUM_LENS_X; c < HALF_NUM_LENS_X; ++c) {
//			int col = c + HALF_NUM_LENS_X;
//			int idx = row * NUM_LENS_X + col;
//			float cx = camPos[idx].x; // s に相当
//			float cy = camPos[idx].y; // t に相当
//			baseOffset[idx] = glm::vec2(cx * scaleX, cy * scaleY); // 符号は座標系に依存。見え方に応じて ± を調整
//		}
//	}
//
//	const float uvScaleX = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_X / (float)Z_PLANE_IMG_PX_X; // = BOX_DETAIL_N * NUM_ELEM_IMG_PX_X / Z_PLANE_IMG_PX_X
//	const float uvScaleY = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_Y / (float)Z_PLANE_IMG_PX_Y; // = BOX_DETAIL_N * NUM_ELEM_IMG_PX_Y / Z_PLANE_IMG_PX_Y
//
//	// フレーム処理
//	//------------------------------
//
//	glfwSetFramebufferSizeCallback(gridWin, framebuffer_size_callback);
//
//	float sum_cpu_time = 0.0f;
//	double sum_gpu_time = 0;
//	int numFrame = 0;
//	const float invZo = 1.0f / MIN_OBSERVE_Z;
//	size_t pointCount = 0;
//
//	int rowsPerThread;
//	int startRow, endRow;
//
//	while (!glfwWindowShouldClose(gridWin))
//	{
//
//		processInput(gridWin);
//
//		// GPU時間測定開始
//		GLuint q = 0; glGenQueries(1, &q);
//		glBeginQuery(GL_TIME_ELAPSED, q);
//
//		// 測定開始時刻を記録
//		auto start = std::chrono::high_resolution_clock::now();
//
//		// 1) 各層バッファの初期化
//		vector<thread> threads;
//		rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//
//		for (int i = 0; i < NUM_THREADS; ++i) {
//			startRow = i * rowsPerThread;
//			endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//			threads.emplace_back(zerosBox, startRow, endRow, red, green, blue, numPoints);
//		}
//		for (auto& t : threads) {
//			if (t.joinable()) { t.join(); }
//		}
//		threads.clear();
//
//		// 測定終了時刻を記録
//		auto end = std::chrono::high_resolution_clock::now();
//
//		// GPU時間測定終了
//		glEndQuery(GL_TIME_ELAPSED);
//		GLuint64 ns = 0; glGetQueryObjectui64v(q, GL_QUERY_RESULT, &ns);
//		double gpuMs = ns / 1.0e6;
//		glDeleteQueries(1, &q);
//
//		// 2) 点群のビニング（幾何変換 → 量子化 → 近傍書き込み）
//		for (int k = 0; k < NUM_POINTS * 3; k += 3) {
//			float tmp_pcd_x = pointsPos[k + 0];
//			float tmp_pcd_y = pointsPos[k + 1];
//			float tmp_pcd_z = pointsPos[k + 2];
//			unsigned char tmp_pcd_b = pointsCol[k + 2];
//			unsigned char tmp_pcd_g = pointsCol[k + 1];
//			unsigned char tmp_pcd_r = pointsCol[k + 0];
//
//			float tmp_zt = 1.0f / tmp_pcd_z;
//			float tmp_xt = tmp_pcd_x * tmp_zt;
//			float tmp_yt = tmp_pcd_y * tmp_zt;
//
//			int tmp_nx = (int)lroundf(F_OVER_Z_PLANE_PITCH * tmp_xt) + HALF_Z_PLANE_IMG_PX_X;
//			int tmp_ny = (int)lroundf(F_OVER_Z_PLANE_PITCH * tmp_yt) + HALF_Z_PLANE_IMG_PX_Y;
//			int tmp_nz = static_cast<int>(std::floor(COEF_TRANS * tmp_zt + 0.5));
//
//			if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
//				for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; ++m) {
//					for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; ++n) {
//						int x = tmp_nx + n;
//						int y = tmp_ny + m;
//						if (0 <= x && x < Z_PLANE_IMG_PX_X && 0 <= y && y < Z_PLANE_IMG_PX_Y) {
//							const int idx = (tmp_nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//							blue[idx] += static_cast<unsigned int>(tmp_pcd_b);
//							green[idx] += static_cast<unsigned int>(tmp_pcd_g);
//							red[idx] += static_cast<unsigned int>(tmp_pcd_r);
//							numPoints[idx]++;
//						}
//					}
//				}
//			}
//		}
//
//		//rowsPerThread = NUM_POINTS / NUM_THREADS;
//		//for (int i = 0; i < NUM_THREADS * 3; i += 3) {
//		//	startRow = i * rowsPerThread;
//		//	endRow = (i == NUM_THREADS * 3 - 1) ? (NUM_POINTS * 3) : (i + 3) * rowsPerThread;
//		//	threads.emplace_back(binningPointClouds, startRow, endRow, pointsPos, pointsCol, red, green, blue, numPoints);
//		//}
//		//for (auto& t : threads) if (t.joinable()) t.join();
//		//threads.clear();
//
//		// 3) 各画素の平均化（並列）
//		rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//		for (int i = 0; i < NUM_THREADS; ++i) {
//			startRow = i * rowsPerThread;
//			endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//			threads.emplace_back(averageSlices, startRow, endRow, red, green, blue, numPoints);
//		}
//		for (auto& t : threads) if (t.joinable()) t.join();
//		threads.clear();
//
//
//		// 4) 各層をテクスチャ配列へアップロード（RGBA8, A=存在フラグ）
//		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//		// まずCPUで全層RGBAを並列パック（OpenGLは呼ばない）
//		threads.clear();
//		rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//		for (int i = 0; i < NUM_THREADS; ++i) {
//			startRow = i * rowsPerThread;
//			endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//			threads.emplace_back(packSlicesRGBA, startRow, endRow,
//				red, green, blue, numPoints, rgba3D.data());
//		}
//		for (auto& t : threads) if (t.joinable()) t.join();
//		threads.clear();
//
//		// メインスレッドで一括アップロード（zoffset=0, depth=NUM_Z_PLANE）
//		glTexSubImage3D(
//			GL_TEXTURE_2D_ARRAY, 0,
//			0, 0, 0,
//			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE,
//			GL_RGBA, GL_UNSIGNED_BYTE, rgba3D.data()
//		);
//		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
//
//		// 5) 合成パス（各タイル=1ドロー、シェーダ内で60層ループ＋早期終了）
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		compose.use();
//		compose.setInt("uSlices", 0);
//		compose.setInt("uNZ", NZ);
//		compose.setVec2("uUvScale", glm::vec2(uvScaleX, uvScaleY));
//		compose.setVec2("uTexelSize", glm::vec2(1.0f / Z_PLANE_IMG_PX_X, 1.0f / Z_PLANE_IMG_PX_Y));
//
//		// invZ はフレーム毎に一度設定（UBOがない前提）
//		glUniform1fv(locInvZ, NZ, invZ.data());
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
//
//		glEnable(GL_SCISSOR_TEST);
//		glBindVertexArray(quadVAO);
//
//		for (int r = -HALF_NUM_LENS_Y; r < HALF_NUM_LENS_Y; ++r) {
//			int row = r + HALF_NUM_LENS_Y;
//			for (int c = -HALF_NUM_LENS_X; c < HALF_NUM_LENS_X; ++c) {
//				int col = c + HALF_NUM_LENS_X;
//				int idx = row * NUM_LENS_X + col;
//
//				int vx = lround(col * FLOAT_NUM_ELEM_IMG_PX_X);
//				int vy = lround(row * FLOAT_NUM_ELEM_IMG_PX_Y);
//
//				glViewport(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//				glScissor(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
//
//				compose.setVec2("uBaseOffset", baseOffset[idx]);
//				glm::vec2 frustumUVShift = baseOffset[idx] * invZo;
//				compose.setVec2("uFrustumUVShift", frustumUVShift);
//
//				glDrawArrays(GL_TRIANGLES, 0, 6);
//			}
//		}
//
//		glBindVertexArray(0);
//		glDisable(GL_SCISSOR_TEST);
//		glUseProgram(0);
//
//		glfwSwapBuffers(gridWin);
//		glfwPollEvents();
//
//		// 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//		sum_cpu_time += duration.count();
//		sum_gpu_time += gpuMs;
//		numFrame++;
//
//		if (numFrame == 100) glfwSetWindowShouldClose(gridWin, true);
//
//	}
//
//	cout << "フレーム数:" << numFrame << endl;
//	cout << "平均CPU実行時間: " << sum_cpu_time / numFrame << " ms" << endl;
//	cout << "平均GPU実行時間: " << sum_gpu_time / numFrame << " ms" << endl;
//
//	//------------------------------
//
//
//	//// 表示画像の保存
//	////------------------------------
//
//	//vector<unsigned char> buf(NUM_DISPLAY_IMG_PX_X * NUM_DISPLAY_IMG_PX_Y * 3);
//	//glReadBuffer(GL_FRONT);
//	//glReadPixels(0, 0, NUM_DISPLAY_IMG_PX_X, NUM_DISPLAY_IMG_PX_Y, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
//
//	//cv::Mat img(NUM_DISPLAY_IMG_PX_Y, NUM_DISPLAY_IMG_PX_X, CV_8UC3);
//	//for (int y = 0; y < NUM_DISPLAY_IMG_PX_Y; ++y) {
//	//	unsigned char* dst = img.ptr<unsigned char>(NUM_DISPLAY_IMG_PX_Y - 1 - y);
//	//	const unsigned char* src = buf.data() + y * NUM_DISPLAY_IMG_PX_X * 3;
//	//	for (int x = 0; x < NUM_DISPLAY_IMG_PX_X; ++x) {
//	//		dst[x * 3 + 0] = src[x * 3 + 2]; // B
//	//		dst[x * 3 + 1] = src[x * 3 + 1]; // G
//	//		dst[x * 3 + 2] = src[x * 3 + 0]; // R
//	//	}
//	//}
//
//	//std::ostringstream stream;
//	//stream << "D:/ForStudy/reconstruction/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1-pepper_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << ".png";
//	//std::string outPath = stream.str();
//
//	//cv::imwrite(outPath, img);
//
//	////------------------------------
//
//	//// 3次元配列のテクスチャ画像の保存
//	////------------------------------
//
//	std::ostringstream stream;
//	cv::Mat sliceImage = cv::Mat::zeros(Z_PLANE_IMG_PX_Y, Z_PLANE_IMG_PX_X, CV_8UC3);
//	int count_color;
//	for (int n = 0; n < NUM_Z_PLANE; n++)
//	{
//		stream.str("");
//		stream << "D:/ForStudy/slice-images/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1-pepper_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << "_nz" << n << ".png";
//
//		count_color = 0;
//		sliceImage = cv::Scalar(0);
//
//		for (int y = 0; y < Z_PLANE_IMG_PX_Y; y++)
//		{
//			int idx_y = Z_PLANE_IMG_PX_Y - y - 1;
//			for (int x = 0; x < Z_PLANE_IMG_PX_X; x++)
//			{
//				int idx = (n * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//				sliceImage.at<cv::Vec3b>(idx_y, x)[0] = blue[idx];
//				sliceImage.at<cv::Vec3b>(idx_y, x)[1] = green[idx];
//				sliceImage.at<cv::Vec3b>(idx_y, x)[2] = red[idx];
//				
//				if (blue[idx] + green[idx] + red[idx] > 0)
//				{
//					count_color++;
//				}
//			}
//		}
//
//		if (count_color > 0)
//		{
//			std::string outPath = stream.str();
//			cv::imwrite(outPath, sliceImage);
//		}
//
//	}
//
//
//
//
//	////------------------------------
//
//	// 後処理
//	//------------------------------
//
//	glDeleteVertexArrays(1, &quadVAO);
//	glDeleteBuffers(1, &quadVBO);
//	glDeleteTextures(1, &texSlices);
//
//	delete[] camPos;
//	delete[] pointsPos;
//	delete[] pointsCol;
//	delete[] red;
//	delete[] green;
//	delete[] blue;
//	delete[] numPoints;
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
//
////void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints) {
////
////	const size_t planePixels = size_t(Z_PLANE_IMG_PX_X) * Z_PLANE_IMG_PX_Y;
////	const size_t offset = size_t(start) * planePixels;
////	const size_t count = size_t(end - start) * planePixels;
////
////	std::memset(red + offset, 0, count * sizeof(unsigned int));
////	std::memset(green + offset, 0, count * sizeof(unsigned int));
////	std::memset(blue + offset, 0, count * sizeof(unsigned int));
////	std::memset(numPoints + offset, 0, count * sizeof(unsigned int));
////}
//
//void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints) {
//
//	const size_t planePixels = size_t(Z_PLANE_IMG_PX_X) * Z_PLANE_IMG_PX_Y;
//	const size_t offset = size_t(start) * planePixels;
//	const size_t count = size_t(end - start) * planePixels;
//
//	std::memset(red + offset, 0, count * sizeof(unsigned int));
//	std::memset(green + offset, 0, count * sizeof(unsigned int));
//	std::memset(blue + offset, 0, count * sizeof(unsigned int));
//	std::memset(numPoints + offset, 0, count * sizeof(unsigned int));
//}
//
//
//void averageSlices(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, const unsigned int* numPoints) {
//
//	for (int i = start; i < end; ++i) {
//		const int base = i * Z_PLANE_IMG_PX_Y * Z_PLANE_IMG_PX_X;
//		for (int j = 0; j < Z_PLANE_IMG_PX_Y; ++j) {
//			for (int k = 0; k < Z_PLANE_IMG_PX_X; ++k) {
//				const int idx = base + j * Z_PLANE_IMG_PX_X + k;
//				const unsigned int cnt = numPoints[idx];
//				if (cnt) {
//					// 四捨五入の整数版
//					red[idx] = (red[idx] + (cnt >> 1)) / cnt;
//					green[idx] = (green[idx] + (cnt >> 1)) / cnt;
//					blue[idx] = (blue[idx] + (cnt >> 1)) / cnt;
//				}
//			}
//		}
//	}
//}
//
//void packSlicesRGBA(int start, int end, const unsigned int* red, const unsigned int* green, const unsigned int* blue, const unsigned int* numPoints, unsigned char* rgba3D)
//{
//	const int layerPitch = Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * 4; // バイト
//	for (int nz = start; nz < end; ++nz) {
//		unsigned char* layerOut = rgba3D + nz * layerPitch;
//		for (int y = 0; y < Z_PLANE_IMG_PX_Y; ++y) {
//			for (int x = 0; x < Z_PLANE_IMG_PX_X; ++x) {
//				const int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//				const int out = (y * Z_PLANE_IMG_PX_X + x) * 4;
//				layerOut[out + 0] = static_cast<unsigned char>(std::min(red[idx], 255u));
//				layerOut[out + 1] = static_cast<unsigned char>(std::min(green[idx], 255u));
//				layerOut[out + 2] = static_cast<unsigned char>(std::min(blue[idx], 255u));
//				layerOut[out + 3] = (numPoints[idx] > 0u) ? 255u : 0u; // A=存在フラグ
//			}
//		}
//	}
//}
//
//void binningPointClouds(int start, int end, const float* pointsPos, const unsigned char* pointsCol, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints)
//{
//	for (int k = start; k < end; k += 3) {
//		float tmp_pcd_x = pointsPos[k + 0];
//		float tmp_pcd_y = pointsPos[k + 1];
//		float tmp_pcd_z = pointsPos[k + 2];
//		unsigned char tmp_pcd_b = pointsCol[k + 2];
//		unsigned char tmp_pcd_g = pointsCol[k + 1];
//		unsigned char tmp_pcd_r = pointsCol[k + 0];
//
//		float tmp_zt = 1.0f / tmp_pcd_z;
//		float tmp_xt = tmp_pcd_x * tmp_zt;
//		float tmp_yt = tmp_pcd_y * tmp_zt;
//
//		int tmp_nx = (int)lroundf(F_OVER_Z_PLANE_PITCH * tmp_xt) + HALF_Z_PLANE_IMG_PX_X;
//		int tmp_ny = (int)lroundf(F_OVER_Z_PLANE_PITCH * tmp_yt) + HALF_Z_PLANE_IMG_PX_Y;
//		int tmp_nz = static_cast<int>(std::floor(COEF_TRANS * tmp_zt));
//
//		if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
//			for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; ++m) {
//				for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; ++n) {
//					int x = tmp_nx + n;
//					int y = tmp_ny + m;
//					if (0 <= x && x < Z_PLANE_IMG_PX_X && 0 <= y && y < Z_PLANE_IMG_PX_Y) {
//						const int idx = (tmp_nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//						blue[idx] += static_cast<unsigned int>(tmp_pcd_b);
//						green[idx] += static_cast<unsigned int>(tmp_pcd_g);
//						red[idx] += static_cast<unsigned int>(tmp_pcd_r);
//						numPoints[idx]++;
//					}
//				}
//			}
//		}
//	}
//}