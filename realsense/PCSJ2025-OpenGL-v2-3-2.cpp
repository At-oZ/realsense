///*
//	PCSJ 2025向けコード(OpenGL導入バージョン)
//	v2-3-2:v2-3からの派生;簡易なリアルタイム位置合わせを導入
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
//#include <algorithm>
//
//// ここから追加/修正
//#ifndef NOMINMAX
//#define NOMINMAX
//#endif
//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL   // ← コンテキストAPI(WGL)を明示
//#include <GLFW/glfw3native.h>
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
//const unsigned int NUM_LENS_X = 40;
//const unsigned int NUM_LENS_Y = 30;
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
//// （置換）旧 centerDinstance 定義をランタイム変更可能な変数へ
//// const float centerDinstance = 0.016f; ←削除
//const float INITIAL_CENTER_DISTANCE = 0.016f; // 初期ギャップ(m)
//float centerDistance = INITIAL_CENTER_DISTANCE; // 変更可能ギャップ(m)
//const float CENTER_DISTANCE_STEP_FINE = 0.001f;
//const float CENTER_DISTANCE_STEP_COARSE = 0.005f;
//
//enum class ControlMode { WindowMove, CenterAdjust };
//ControlMode controlMode = ControlMode::WindowMove;
//bool gapDirty = true; // 初回再計算要求
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
//const int Z_PLANE_IMG_PX_X = 600;
//const int Z_PLANE_IMG_PX_Y = 480;
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
//// 窓移動状態
//struct WindowMoveState {
//	int prevUp = GLFW_RELEASE;
//	int prevDown = GLFW_RELEASE;
//	int prevLeft = GLFW_RELEASE;
//	int prevRight = GLFW_RELEASE;
//};
//
//// --- Gamepad用エッジ検出状態 ---
//struct GamepadEdgeState { unsigned char up = 0, down = 0, left = 0, right = 0, x = 0, back = 0, y = 0, a = 0; };
//
//// プロトタイプ
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//static void printGLInfo(const char* tag);
//static GLFWmonitor* getMonitorByIndex(int index);
//void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
//void packSlicesRGBA(int start, int end,
//	const unsigned int* red, const unsigned int* green, const unsigned int* blue,
//	const unsigned int* numPoints, unsigned char* rgba3D);
//void averageSlices(int start, int end,
//	unsigned int* red, unsigned int* green, unsigned int* blue,
//	const unsigned int* numPoints);
//void binningPointClouds(int start, int end,
//	const float* pointsPos, const unsigned char* pointsCol,
//	unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
//void zerosByIndexRange(int begin, int end, const int* indices,
//	unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
//static void updateWindowPositionDiscrete(
//	GLFWwindow* window,
//	int& offsetX, int& offsetY,
//	int monX, int monY,
//	int upKey, int downKey, int leftKey, int rightKey,
//	WindowMoveState& state);
//static void toggleFocusWithKey(GLFWwindow* winLeft, GLFWwindow* winRight);
//static void focusWindowPortable(GLFWwindow* w);
//
//static int  selectPrimaryGamepad();
//static void logAllJoysticksOnce();
//static void logJoystickButtonsAxes(int jid);
//static void processGamepadDiscrete(GLFWwindow* winLeft, GLFWwindow* winRight,
//	int& offsetXLeft, int& offsetYLeft,
//	int& offsetXRight, int& offsetYRight,
//	int monX, int monY, GamepadEdgeState& gp,
//	int gamepadIndex,
//	float& centerDistance, bool& gapDirty, ControlMode& controlMode);
//
//static void drawTiles(
//	GLFWwindow* win, Shader& compose,
//	const glm::vec2* baseOffset, int lensColsStart, int lensColsCount,
//	int lensRowsStart, int lensRowsCount,
//	int tileW, int tileH,
//	int halfCols, int halfRows,
//	int nz, GLint locInvZ, const float* invZ, float uvScaleX, float uvScaleY,
//	int texW, int texH, float invZo, GLuint texSlices, GLuint quadVAO);
//
//static inline void printMoveAmounts(int offsetX, int offsetY);
//static void recomputeCameraLayout(float gap,
//	glm::vec3* camPos,
//	std::vector<glm::vec2>& baseOffset,
//	int numLensX, int numLensY,
//	int halfX, int halfY,
//	float halfPitchX, float halfPitchY,
//	float focalLen, float imgPitch, int texW, int texH);
//
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
//	// 左右分割設定
//	const int LEFT_COLS = NUM_LENS_X / 2;           // 偶数前提
//	const int RIGHT_COLS = NUM_LENS_X - LEFT_COLS;  // 念のため
//	const int leftWinW = LEFT_COLS * NUM_ELEM_IMG_PX_X;
//	const int rightWinW = RIGHT_COLS * NUM_ELEM_IMG_PX_X;
//	const int winH = NUM_LENS_Y * NUM_ELEM_IMG_PX_Y;
//
//	// 各ウィンドウの原点オフセット（モニタ基準）
//	int offsetXLeft = 0;
//	int offsetYLeft = 0;
//	// 右ウィンドウは左の右隣に初期配置
//	int offsetXRight = leftWinW; // 連結表示
//	int offsetYRight = 0;
//
//	// 左ウィンドウ作成
//	GLFWwindow* winLeft = glfwCreateWindow(leftWinW, winH, "Grid Cameras Left", nullptr, nullptr);
//	if (!winLeft) { std::fprintf(stderr, "Create left window failed\n"); glfwTerminate(); return -1; }
//	glfwMakeContextCurrent(winLeft);
//	glfwSwapInterval(1);
//	// （追加）右用VAO/VBOを保持するグローバル（左作成部のすぐ下あたりに挿入）
//	GLuint quadVAORight = 0, quadVBORight = 0;
//
//	int mx = 0, my = 0;
//	glfwGetMonitorPos(mon, &mx, &my);
//	glfwSetWindowPos(winLeft, mx + offsetXLeft, my + offsetYLeft);
//#ifdef GLFW_FLOATING
//	glfwSetWindowAttrib(winLeft, GLFW_FLOATING, GLFW_TRUE);
//#endif
//	glfwShowWindow(winLeft);
//
//	// GLAD（最初のコンテキストで一度）
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::fprintf(stderr, "GLAD init failed\n"); return -1;
//	}
//	printGLInfo("LeftWindow");
//
//	// 右ウィンドウ（共有コンテキスト）
//	GLFWwindow* winRight = glfwCreateWindow(rightWinW, winH, "Grid Cameras Right", nullptr, winLeft);
//	if (!winRight) { std::fprintf(stderr, "Create right window failed\n"); glfwTerminate(); return -1; }
//	glfwSetWindowPos(winRight, mx + offsetXRight, my + offsetYRight);
//#ifdef GLFW_FLOATING
//	glfwSetWindowAttrib(winRight, GLFW_FLOATING, GLFW_TRUE);
//#endif
//	glfwShowWindow(winRight);
//
//	// 一度ジョイスティック一覧をログ & 使用ID決定
//	logAllJoysticksOnce();
//	int gamepadIndex = selectPrimaryGamepad();
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
//	cv::Mat image_input = cv::imread("./images/standard/parrots.bmp");
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
//	std::vector<glm::vec2> baseOffset(NUM_LENS_X * NUM_LENS_Y); // 先に確保
//	recomputeCameraLayout(centerDistance,
//		camPos,
//		baseOffset,
//		NUM_LENS_X, NUM_LENS_Y,
//		HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
//		HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y,
//		FOCAL_LENGTH, Z_PLANE_IMG_PITCH,
//		Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);
//
//	//glm::vec3* camPos = new glm::vec3[NUM_LENS_X * NUM_LENS_Y];
//	//for (int r = -HALF_NUM_LENS_Y; r < HALF_NUM_LENS_Y; ++r)
//	//{
//	//	int row = r + HALF_NUM_LENS_Y;
//	//	float camPosY = (2.0f * (float)r + 1.0f) * HALF_LENS_PITCH_Y;
//
//	//	for (int c = -HALF_NUM_LENS_X; c < HALF_NUM_LENS_X; ++c)
//	//	{
//	//		int col = c + HALF_NUM_LENS_X;
//	//		float camPosX = (2.0f * (float)c + 1.0f) * HALF_LENS_PITCH_X;
//
//	//		// （追加）左右ウィンドウ分割境界（列 -1 と 0 の間）にギャップを挿入
//	//		// c < 0 側を負方向へ centerDistance/2、c >= 0 側を正方向へ centerDistance/2 シフト
//	//		if (centerDistance != 0.0f) {
//	//			camPosX += (c >= 0 ? +0.5f : -0.5f) * centerDistance;
//	//		}
//
//	//		camPos[row * NUM_LENS_X + col] = glm::vec3(camPosX, camPosY, 0.0f);
//	//	}
//	//}
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
//	// （修正）左側VAO/VBO作成直後に右側コンテキストで複製作成（VAOは共有されないため）
//	// VBO, VAOの作成（左）
//	{
//		const float quad[] = {
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
//	// （追加）右コンテキストでVAO/VBO複製
//	glfwMakeContextCurrent(winRight);
//	{
//		const float quad[] = {
//			-1.f, -1.f, 0.f, 0.f,
//			 1.f, -1.f, 1.f, 0.f,
//			 1.f,  1.f, 1.f, 1.f,
//			-1.f, -1.f, 0.f, 0.f,
//			 1.f,  1.f, 1.f, 1.f,
//			-1.f,  1.f, 0.f, 1.f,
//		};
//		glGenVertexArrays(1, &quadVAORight);
//		glGenBuffers(1, &quadVBORight);
//		glBindVertexArray(quadVAORight);
//		glBindBuffer(GL_ARRAY_BUFFER, quadVBORight);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindVertexArray(0);
//	}
//	// 左を再度アクティブに戻す
//	glfwMakeContextCurrent(winLeft);
//
//	//------------------------------
//
//
//	unsigned int* red = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* green = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* blue = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//	unsigned int* numPoints = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
//
//	const size_t TOTAL_PIXELS = size_t(Z_PLANE_IMG_PX_X) * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE;
//
//	// 前フレーム/今フレームの dirty リスト
//	std::vector<int> dirtyIndices;
//	std::vector<int> nextDirtyIndices;
//	dirtyIndices.reserve(TOTAL_PIXELS / 4);
//	nextDirtyIndices.reserve(TOTAL_PIXELS / 4);
//
//	// 初回だけ全域0クリアするフラグ
//	bool firstFrame = true;
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
//	//std::vector<glm::vec2> baseOffset(NUM_LENS_X * NUM_LENS_Y);
//	//for (int r = -HALF_NUM_LENS_Y; r < HALF_NUM_LENS_Y; ++r) {
//	//	int row = r + HALF_NUM_LENS_Y;
//	//	for (int c = -HALF_NUM_LENS_X; c < HALF_NUM_LENS_X; ++c) {
//	//		int col = c + HALF_NUM_LENS_X;
//	//		int idx = row * NUM_LENS_X + col;
//	//		float cx = camPos[idx].x; // s に相当
//	//		float cy = camPos[idx].y; // t に相当
//	//		baseOffset[idx] = glm::vec2(cx * scaleX, cy * scaleY); // 符号は座標系に依存。見え方に応じて ± を調整
//	//	}
//	//}
//
//	const float uvScaleX = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_X / (float)Z_PLANE_IMG_PX_X; // = BOX_DETAIL_N * NUM_ELEM_IMG_PX_X / Z_PLANE_IMG_PX_X
//	const float uvScaleY = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_Y / (float)Z_PLANE_IMG_PX_Y; // = BOX_DETAIL_N * NUM_ELEM_IMG_PX_Y / Z_PLANE_IMG_PX_Y
//
//	// フレーム処理
//	//------------------------------
//
//	glfwSetFramebufferSizeCallback(winLeft, framebuffer_size_callback);
//	glfwSetFramebufferSizeCallback(winRight, framebuffer_size_callback);
//
//	float sum_cpu_time = 0.0f;
//	double sum_gpu_time = 0.0;
//	int numFrame = 0;
//	const float invZo = 1.0f / MIN_OBSERVE_Z;
//
//	int rowsPerThread;
//	int startRow, endRow;
//
//	double lastTime = glfwGetTime();
//
//	WindowMoveState moveLeftState;
//	WindowMoveState moveRightState;
//
//	GamepadEdgeState gpState;
//
//	while (!glfwWindowShouldClose(winLeft) && !glfwWindowShouldClose(winRight))
//	{
//
//
//		processInput(winLeft);
//		processInput(winRight);
//
//		double currentTime = glfwGetTime();
//		float dt = (float)(currentTime - lastTime);
//		lastTime = currentTime;
//		(void)dt; // 今回は離散移動なので未使用
//
//		// 追加: Fキーで左右ウィンドウのフォーカスをトグル
//		toggleFocusWithKey(winLeft, winRight);
//
//		// （変更）メインループ内：processGamepadDiscrete 呼び出しの引数追加 & ギャップ再計算
//		processGamepadDiscrete(winLeft, winRight,
//			offsetXLeft, offsetYLeft,
//			offsetXRight, offsetYRight,
//			mx, my, gpState,
//			gamepadIndex,
//			centerDistance, gapDirty, controlMode);
//
//		// （追加）ギャップ変更があればカメラ / baseOffset 再計算
//		if (gapDirty) {
//			recomputeCameraLayout(centerDistance,
//				camPos,
//				baseOffset,
//				NUM_LENS_X, NUM_LENS_Y,
//				HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
//				HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y,
//				FOCAL_LENGTH, Z_PLANE_IMG_PITCH,
//				Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);
//			gapDirty = false;
//		}
//
//		// 任意間隔で状態ログ（例: 1秒毎）
//		static double lastLog = 0.0;
//		if (gamepadIndex >= 0 && currentTime - lastLog > 1.0) {
//			logJoystickButtonsAxes(gamepadIndex);
//			lastLog = currentTime;
//		}
//
//		// 各ウィンドウ独立移動 (フォーカスがある側しか入力は届かない点に注意)
//		updateWindowPositionDiscrete(winLeft, offsetXLeft, offsetYLeft, mx, my,
//			GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, moveLeftState);
//		updateWindowPositionDiscrete(winRight, offsetXRight, offsetYRight, mx, my,
//			GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, moveRightState);
//
//		// ==== 計測用: 左コンテキストをアクティブにして計測 ====
//		glfwMakeContextCurrent(winLeft);
//		GLuint q = 0; glGenQueries(1, &q);
//		glBeginQuery(GL_TIME_ELAPSED, q);
//		auto start = std::chrono::high_resolution_clock::now();
//
//		// 1) クリア(差分)
//		std::vector<std::thread> threads;
//		if (firstFrame) {
//			std::memset(red, 0, TOTAL_PIXELS * sizeof(unsigned int));
//			std::memset(green, 0, TOTAL_PIXELS * sizeof(unsigned int));
//			std::memset(blue, 0, TOTAL_PIXELS * sizeof(unsigned int));
//			std::memset(numPoints, 0, TOTAL_PIXELS * sizeof(unsigned int));
//			firstFrame = false;
//		}
//		else {
//			const size_t N = dirtyIndices.size();
//			if (N) {
//				const size_t chunk = (N + NUM_THREADS - 1) / NUM_THREADS;
//				for (int i = 0; i < NUM_THREADS; ++i) {
//					size_t begin = size_t(i) * chunk;
//					if (begin >= N) break;
//					size_t end = (std::min)(N, begin + chunk);
//					threads.emplace_back(zerosByIndexRange,
//						(int)begin, (int)end, dirtyIndices.data(),
//						red, green, blue, numPoints);
//				}
//				for (auto& t : threads) if (t.joinable()) t.join();
//			}
//			dirtyIndices.clear();
//		}
//
//		//vector<thread> threads;
//		//rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//
//		//for (int i = 0; i < NUM_THREADS; ++i) {
//		//	startRow = i * rowsPerThread;
//		//	endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//		//	threads.emplace_back(zerosBox, startRow, endRow, red, green, blue, numPoints);
//		//}
//		//for (auto& t : threads) {
//		//	if (t.joinable()) { t.join(); }
//		//}
//		//threads.clear();
//
//		// 2) ビニング
//		for (int k = 0; k < NUM_POINTS * 3; k += 3) {
//			float px = pointsPos[k + 0];
//			float py = pointsPos[k + 1];
//			float pz = pointsPos[k + 2];
//			unsigned char b = pointsCol[k + 2];
//			unsigned char g = pointsCol[k + 1];
//			unsigned char r = pointsCol[k + 0];
//
//			float invz = 1.0f / pz;
//			float xt = px * invz;
//			float yt = py * invz;
//
//			int nx = (int)lroundf(F_OVER_Z_PLANE_PITCH * xt) + HALF_Z_PLANE_IMG_PX_X;
//			int ny = (int)lroundf(F_OVER_Z_PLANE_PITCH * yt) + HALF_Z_PLANE_IMG_PX_Y;
//			int nz = static_cast<int>(std::floor(COEF_TRANS * invz + 0.5f));
//
//			if (0 <= nz && nz < NUM_Z_PLANE) {
//				for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; ++m) {
//					for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; ++n) {
//						int x = nx + n;
//						int y = ny + m;
//						if (0 <= x && x < Z_PLANE_IMG_PX_X && 0 <= y && y < Z_PLANE_IMG_PX_Y) {
//							int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//							if (numPoints[idx] == 0u) nextDirtyIndices.push_back(idx);
//							blue[idx] += (unsigned int)b;
//							green[idx] += (unsigned int)g;
//							red[idx] += (unsigned int)r;
//							numPoints[idx] += 1u;
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
//		// 3) 平均化
//		rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//		for (int i = 0; i < NUM_THREADS; ++i) {
//			startRow = i * rowsPerThread;
//			endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//			threads.emplace_back(averageSlices, startRow, endRow, red, green, blue, numPoints);
//		}
//		for (auto& t : threads) if (t.joinable()) t.join();
//		threads.clear();
//
//		// 4) RGBAパック
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
//		// 4.5) GPU転送 (左コンテキストでOK → 共有される)
//		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
//			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE,
//			GL_RGBA, GL_UNSIGNED_BYTE, rgba3D.data());
//		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
//
//		// （差し替え）タイル描画呼び出し
//		drawTiles(winLeft, compose, baseOffset.data(),
//			-HALF_NUM_LENS_X, LEFT_COLS,
//			-HALF_NUM_LENS_Y, NUM_LENS_Y,
//			NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y,
//			HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
//			NZ, locInvZ, invZ.data(), uvScaleX, uvScaleY,
//			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, invZo, texSlices, quadVAO);
//
//		// （追加）デバッグ: エラー表示
//		auto dumpErr = [](const char* tag) {
//			GLenum e; while ((e = glGetError()) != GL_NO_ERROR) { std::cout << "[GLERR " << tag << "] 0x" << std::hex << e << std::dec << "\n"; }
//			};
//		dumpErr("beforeRightDraw");
//
//		// （修正）右側描画呼び出し：locInvZ を右コンテキストで再取得し右VAOを使用（drawTiles 呼び出し部）
//		GLint locInvZRight = 0;
//		glfwMakeContextCurrent(winRight);
//		locInvZRight = glGetUniformLocation(compose.ID, "uInvZ");
//		glfwMakeContextCurrent(winLeft); // 左へ戻してから関数内で再度切替
//
//		drawTiles(winRight, compose, baseOffset.data(),
//			0, RIGHT_COLS,
//			-HALF_NUM_LENS_Y, NUM_LENS_Y,
//			NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y,
//			HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
//			NZ, locInvZRight, invZ.data(), uvScaleX, uvScaleY,
//			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, invZo, texSlices, quadVAORight);
//
//		dumpErr("afterRightDraw");
//
//		// GPU計測終了（左に戻して取得）
//		glfwMakeContextCurrent(winLeft);
//		glEndQuery(GL_TIME_ELAPSED);
//		GLuint64 ns = 0; glGetQueryObjectui64v(q, GL_QUERY_RESULT, &ns);
//		double gpuMs = ns / 1.0e6;
//		glDeleteQueries(1, &q);
//
//		// イベント
//		glfwPollEvents();
//
//		// 次フレーム用 dirty
//		dirtyIndices.swap(nextDirtyIndices);
//		nextDirtyIndices.clear();
//
//		// CPU時間計測終了
//		auto end = std::chrono::high_resolution_clock::now();
//
//		// 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//		if (numFrame > 0)
//		{
//			sum_cpu_time += duration.count();
//			sum_gpu_time += gpuMs;
//		}
//
//		numFrame++;
//
//		//if (numFrame == 101) glfwSetWindowShouldClose(gridWin, true);
//
//	}
//
//	std::cout << "フレーム数:" << numFrame << std::endl;
//	if (numFrame > 1) {
//		std::cout << "平均CPU実行時間: " << sum_cpu_time / (numFrame - 1) << " ms" << std::endl;
//		std::cout << "平均GPU実行時間: " << sum_gpu_time / (numFrame - 1) << " ms" << std::endl;
//	}
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
//	//std::ostringstream stream;
//	//cv::Mat sliceImage = cv::Mat::zeros(Z_PLANE_IMG_PX_Y, Z_PLANE_IMG_PX_X, CV_8UC3);
//	//int count_color;
//	//for (int n = 0; n < NUM_Z_PLANE; n++)
//	//{
//	//	stream.str("");
//	//	stream << "D:/ForStudy/slice-images/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1-pepper_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << "_nz" << n << ".png";
//
//	//	count_color = 0;
//	//	sliceImage = cv::Scalar(0);
//
//	//	for (int y = 0; y < Z_PLANE_IMG_PX_Y; y++)
//	//	{
//	//		int idx_y = Z_PLANE_IMG_PX_Y - y - 1;
//	//		for (int x = 0; x < Z_PLANE_IMG_PX_X; x++)
//	//		{
//	//			int idx = (n * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
//	//			sliceImage.at<cv::Vec3b>(idx_y, x)[0] = blue[idx];
//	//			sliceImage.at<cv::Vec3b>(idx_y, x)[1] = green[idx];
//	//			sliceImage.at<cv::Vec3b>(idx_y, x)[2] = red[idx];
//
//	//			if (blue[idx] + green[idx] + red[idx] > 0)
//	//			{
//	//				count_color++;
//	//			}
//	//		}
//	//	}
//
//	//	if (count_color > 0)
//	//	{
//	//		std::string outPath = stream.str();
//	//		cv::imwrite(outPath, sliceImage);
//	//	}
//
//	//}
//
//
//
//
//	////------------------------------
//
//	// 後処理
//	//------------------------------
//
//// （修正）後処理：右VAO/VBOも破棄
//	glDeleteVertexArrays(1, &quadVAO);
//	glDeleteBuffers(1, &quadVBO);
//	glDeleteVertexArrays(1, &quadVAORight);
//	glDeleteBuffers(1, &quadVBORight);
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
//void zerosByIndexRange(int begin, int end, const int* indices,
//	unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints)
//{
//	for (int i = begin; i < end; ++i) {
//		const int idx = indices[i];
//		red[idx] = 0u;
//		green[idx] = 0u;
//		blue[idx] = 0u;
//		numPoints[idx] = 0u;
//	}
//}
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
//				layerOut[out + 0] = static_cast<unsigned char>((std::min)(red[idx], 255u));
//				layerOut[out + 1] = static_cast<unsigned char>((std::min)(green[idx], 255u));
//				layerOut[out + 2] = static_cast<unsigned char>((std::min)(blue[idx], 255u));
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
//
//// 離散1回押し移動（1px / Shift同時=10px）
//static void updateWindowPositionDiscrete(
//	GLFWwindow* window, int& offsetX, int& offsetY, int monX, int monY,
//	int upKey, int downKey, int leftKey, int rightKey, WindowMoveState& state)
//{
//	int curUp = glfwGetKey(window, upKey);
//	int curDown = glfwGetKey(window, downKey);
//	int curLeft = glfwGetKey(window, leftKey);
//	int curRight = glfwGetKey(window, rightKey);
//	bool shiftHeld =
//		(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
//		(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
//
//	int step = shiftHeld ? 10 : 1;
//	auto pressedOnce = [](int cur, int prev) { return prev == GLFW_RELEASE && cur == GLFW_PRESS; };
//
//	bool moved = false;
//	if (pressedOnce(curUp, state.prevUp)) offsetY -= step;
//	if (pressedOnce(curDown, state.prevDown)) offsetY += step;
//	if (pressedOnce(curLeft, state.prevLeft)) offsetX -= step;
//	if (pressedOnce(curRight, state.prevRight)) offsetX += step;
//
//	state.prevUp = curUp;
//	state.prevDown = curDown;
//	state.prevLeft = curLeft;
//	state.prevRight = curRight;
//
//	glfwSetWindowPos(window, monX + offsetX, monY + offsetY);
//	if (moved) {
//		printMoveAmounts(offsetX, offsetY);
//	}
//}
//
//static void focusWindowPortable(GLFWwindow* w)
//{
//#if defined(_WIN32)
//	HWND hwnd = glfwGetWin32Window(w);
//	if (hwnd) {
//		ShowWindow(hwnd, SW_SHOW);
//		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//		SetForegroundWindow(hwnd);
//		SetFocus(hwnd);
//		BringWindowToTop(hwnd);
//	}
//#else
//	glfwShowWindow(w);
//#endif
//}
//
//static void toggleFocusWithKey(GLFWwindow* winLeft, GLFWwindow* winRight)
//{
//	static int prevF = GLFW_RELEASE;
//
//	GLFWwindow* focused = nullptr;
//	if (glfwGetWindowAttrib(winLeft, GLFW_FOCUSED) == GLFW_TRUE) focused = winLeft;
//	else if (glfwGetWindowAttrib(winRight, GLFW_FOCUSED) == GLFW_TRUE) focused = winRight;
//	else return;
//
//	const int curF = glfwGetKey(focused, GLFW_KEY_F);
//	if (prevF == GLFW_RELEASE && curF == GLFW_PRESS) {
//		GLFWwindow* target = (focused == winLeft) ? winRight : winLeft;
//		focusWindowPortable(target);
//	}
//	prevF = curF;
//}
//
//static void logAllJoysticksOnce() {
//	std::cout << "[Joystick Scan] Begin\n";
//	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
//		if (glfwJoystickPresent(jid) == GLFW_TRUE) {
//			const char* name = glfwGetJoystickName(jid);
//			bool mapped = glfwJoystickIsGamepad(jid) == GLFW_TRUE;
//			int axisCount = 0; glfwGetJoystickAxes(jid, &axisCount);
//			int btnCount = 0; glfwGetJoystickButtons(jid, &btnCount);
//			int hatCount = 0; glfwGetJoystickHats(jid, &hatCount);
//			std::cout << "  jid=" << jid << " name='" << (name ? name : "null") << "' mapped=" << (mapped ? "YES" : "NO")
//				<< " axes=" << axisCount << " buttons=" << btnCount << " hats=" << hatCount << "\n";
//		}
//	}
//	std::cout << "[Joystick Scan] End\n";
//}
//
//static int selectPrimaryGamepad() {
//	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
//		if (glfwJoystickPresent(jid) == GLFW_TRUE) {
//			if (glfwJoystickIsGamepad(jid) == GLFW_TRUE) { std::cout << "[GamepadSelect] Use mapped " << jid << "\n"; return jid; }
//			if (jid == GLFW_JOYSTICK_1) return jid; // fallback
//		}
//	}
//	std::cout << "[GamepadSelect] No joystick present\n";
//	return -1;
//}
//
//static void logJoystickButtonsAxes(int jid) {
//	if (jid < GLFW_JOYSTICK_1 || jid > GLFW_JOYSTICK_LAST) return;
//	if (glfwJoystickPresent(jid) != GLFW_TRUE) return;
//
//	int axisCount = 0, btnCount = 0, hatCount = 0;
//	const float* axes = glfwGetJoystickAxes(jid, &axisCount);
//	const unsigned char* btns = glfwGetJoystickButtons(jid, &btnCount);
//	const unsigned char* hats = glfwGetJoystickHats(jid, &hatCount);
//
//	//std::cout << "[JoystickState] jid=" << jid << " axes(" << axisCount << "):";
//	//for (int i = 0; i < axisCount; ++i) std::cout << axes[i] << ",";
//	//std::cout << " buttons(" << btnCount << "):";
//	//for (int i = 0; i < btnCount; ++i) std::cout << (int)btns[i];
//	//std::cout << " hats(" << hatCount << "):";
//	//for (int i = 0; i < hatCount; ++i) std::cout << (int)hats[i] << " ";
//	//std::cout << "\n";
//}
//
//// （変更）processGamepadDiscrete シグネチャ拡張（centerDistance操作用フラグ受け渡し）
//static void processGamepadDiscrete(GLFWwindow* winLeft, GLFWwindow* winRight,
//	int& offsetXLeft, int& offsetYLeft,
//	int& offsetXRight, int& offsetYRight,
//	int monX, int monY, GamepadEdgeState& gp,
//	int gamepadIndex,
//	float& centerDistance, bool& gapDirty, ControlMode& controlMode) 
//{
//	if (gamepadIndex < GLFW_JOYSTICK_1 || gamepadIndex > GLFW_JOYSTICK_LAST) return;
//	if (glfwJoystickPresent(gamepadIndex) != GLFW_TRUE) return;
//
//	GLFWwindow* focused = nullptr;
//	if (glfwGetWindowAttrib(winLeft, GLFW_FOCUSED) == GLFW_TRUE) focused = winLeft;
//	else if (glfwGetWindowAttrib(winRight, GLFW_FOCUSED) == GLFW_TRUE) focused = winRight;
//
//	unsigned char curUp = 0, curDown = 0, curLeft = 0, curRight = 0;
//
//	// HAT→DPAD
//	int hatCount = 0;
//	const unsigned char* hats = glfwGetJoystickHats(gamepadIndex, &hatCount);
//	if (hatCount > 0) {
//		unsigned char h = hats[0];
//		curUp |= (h & GLFW_HAT_UP) ? 1 : 0;
//		curRight |= (h & GLFW_HAT_RIGHT) ? 1 : 0;
//		curDown |= (h & GLFW_HAT_DOWN) ? 1 : 0;
//		curLeft |= (h & GLFW_HAT_LEFT) ? 1 : 0;
//	}
//
//	int btnCount = 0;
//	const unsigned char* btns = glfwGetJoystickButtons(gamepadIndex, &btnCount);
//
//	unsigned char curLB = 0, curRB = 0, curX = 0, curBack = 0, curA = 0, curY = 0;
//	if (btnCount >= 6) { curLB = btns[4]; curRB = btns[5]; }
//	if (btnCount >= 3) { curX = btns[2]; }          // X: A=0,B=1,X=2,Y=3
//	if (btnCount >= 7) { curBack = btns[6]; }       // Back (fallback)
//	if (btnCount >= 4) {
//		curA = btns[0];
//		curY = btns[3];
//	}
//
//	// Gamepad API (より確実)
//	GLFWgamepadstate st{};
//	if (glfwJoystickIsGamepad(gamepadIndex) == GLFW_TRUE && glfwGetGamepadState(gamepadIndex, &st) == GLFW_TRUE) {
//		curBack = st.buttons[GLFW_GAMEPAD_BUTTON_BACK];
//		// DPADもこちらを優先
//		curUp = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
//		curRight = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
//		curDown = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
//		curLeft = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
//		curA = st.buttons[GLFW_GAMEPAD_BUTTON_A];
//		curY = st.buttons[GLFW_GAMEPAD_BUTTON_Y];
//	}
//
//	auto pressedOnce = [](unsigned char cur, unsigned char prev) { return prev == 0 && cur == 1; };
//
//	// Xでフォーカス切替
//	if (pressedOnce(curX, gp.x)) {
//		GLFWwindow* target = nullptr;
//		if (focused == nullptr) target = winLeft;
//		else target = (focused == winLeft) ? winRight : winLeft;
//		focusWindowPortable(target);
//		focused = target;
//	}
//	// BackでESC相当
//	if (pressedOnce(curBack, gp.back)) {
//		if (focused) glfwSetWindowShouldClose(focused, true);
//	}
//	// 立ち上がり判定ラムダ（既存 pressedOnce 再利用）
//	if (pressedOnce(curY, gp.y)) {
//		if (controlMode != ControlMode::CenterAdjust) {
//			controlMode = ControlMode::CenterAdjust;
//			std::cout << "[Mode] centerDistance調整モード開始 現在: " << centerDistance << " m\n";
//		}
//	}
//	if (pressedOnce(curA, gp.a)) {
//		if (controlMode != ControlMode::WindowMove) {
//			controlMode = ControlMode::WindowMove;
//			std::cout << "[Mode] ウィンドウ移動モードへ戻る\n";
//		}
//	}
//
//	// （変更）processGamepadDiscrete 内：モード別分岐（フォーカス移動後あたり）
//	if (controlMode == ControlMode::CenterAdjust) {
//		// ギャップ調整のみ。ウィンドウ移動は抑止。
//		float stepGap = (curLB || curRB) ? CENTER_DISTANCE_STEP_COARSE : CENTER_DISTANCE_STEP_FINE;
//		bool adjusted = false;
//		if (pressedOnce(curUp, gp.up)) {
//			centerDistance += stepGap;
//			adjusted = true;
//		}
//		if (pressedOnce(curDown, gp.down)) {
//			centerDistance = std::max(0.0f, centerDistance - stepGap);
//			adjusted = true;
//		}
//		if (adjusted) {
//			gapDirty = true;
//			std::cout << "[centerDistance] 現在: " << centerDistance << " m (step=" << stepGap << ")\n";
//		}
//		// 上下は無視（必要なら上下も別用途で拡張可能）
//	}
//	else {
//		// 既存ウィンドウ移動ロジック（右側のブロック全体がこれに含まれる）
//		int step = (curLB || curRB) ? 10 : 1;
//		if (focused) {
//			if (pressedOnce(curUp, gp.up)) {
//				if (focused == winLeft) { offsetYLeft -= step; glfwSetWindowPos(winLeft, monX + offsetXLeft, monY + offsetYLeft); printMoveAmounts(offsetXLeft, offsetYLeft); }
//				else { offsetYRight -= step; glfwSetWindowPos(winRight, monX + offsetXRight, monY + offsetYRight); printMoveAmounts(offsetXRight, offsetYRight); }
//			}
//			if (pressedOnce(curDown, gp.down)) {
//				if (focused == winLeft) { offsetYLeft += step; glfwSetWindowPos(winLeft, monX + offsetXLeft, monY + offsetYLeft); printMoveAmounts(offsetXLeft, offsetYLeft); }
//				else { offsetYRight += step; glfwSetWindowPos(winRight, monX + offsetXRight, monY + offsetYRight); printMoveAmounts(offsetXRight, offsetYRight); }
//			}
//			if (pressedOnce(curLeft, gp.left)) {
//				if (focused == winLeft) { offsetXLeft -= step; glfwSetWindowPos(winLeft, monX + offsetXLeft, monY + offsetYLeft); printMoveAmounts(offsetXLeft, offsetYLeft); }
//				else { offsetXRight -= step; glfwSetWindowPos(winRight, monX + offsetXRight, monY + offsetYRight); printMoveAmounts(offsetXRight, offsetYRight); }
//			}
//			if (pressedOnce(curRight, gp.right)) {
//				if (focused == winLeft) { offsetXLeft += step; glfwSetWindowPos(winLeft, monX + offsetXLeft, monY + offsetYLeft); printMoveAmounts(offsetXLeft, offsetYLeft); }
//				else { offsetXRight += step; glfwSetWindowPos(winRight, monX + offsetXRight, monY + offsetYRight); printMoveAmounts(offsetXRight, offsetYRight); }
//			}
//		}
//	}
//
//	// （変更）processGamepadDiscrete 内：状態更新に y / a 追加
//	gp.up = curUp; gp.down = curDown; gp.left = curLeft; gp.right = curRight;
//	gp.x = curX; gp.back = curBack; gp.y = curY; gp.a = curA; 
//}
//
//static void drawTiles(
//	GLFWwindow* win, Shader& compose,
//	const glm::vec2* baseOffset, int lensColsStart, int lensColsCount,
//	int lensRowsStart, int lensRowsCount,
//	int tileW, int tileH,
//	int halfCols, int halfRows,
//	int nz, GLint locInvZ, const float* invZ, float uvScaleX, float uvScaleY,
//	int texW, int texH, float invZo, GLuint texSlices, GLuint quadVAO)
//{
//	glfwMakeContextCurrent(win);
//	glClearColor(0.f, 0.f, 0.f, 1.f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	compose.use();
//	compose.setInt("uSlices", 0);
//	compose.setInt("uNZ", nz);
//	compose.setVec2("uUvScale", glm::vec2(uvScaleX, uvScaleY));
//	compose.setVec2("uTexelSize", glm::vec2(1.f / texW, 1.f / texH));
//	glUniform1fv(locInvZ, nz, invZ);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
//
//	glEnable(GL_SCISSOR_TEST);
//	glBindVertexArray(quadVAO);
//
//	for (int r = lensRowsStart; r < lensRowsStart + lensRowsCount; ++r) {
//		int rowIdx = r + halfRows;
//		int localRow = r - lensRowsStart;
//		for (int c = lensColsStart; c < lensColsStart + lensColsCount; ++c) {
//			int lensColIdx = c + halfCols;
//			int localCol = c - lensColsStart;
//			int idx = rowIdx * (halfCols * 2) + lensColIdx;
//
//			int vx = localCol * tileW;
//			int vy = localRow * tileH;
//			glViewport(vx, vy, tileW, tileH);
//			glScissor(vx, vy, tileW, tileH);
//
//			compose.setVec2("uBaseOffset", baseOffset[idx]);
//			glm::vec2 frustumUVShift = baseOffset[idx] * invZo;
//			compose.setVec2("uFrustumUVShift", frustumUVShift);
//
//			glDrawArrays(GL_TRIANGLES, 0, 6);
//		}
//	}
//	glBindVertexArray(0);
//	glDisable(GL_SCISSOR_TEST);
//	glUseProgram(0);
//	glfwSwapBuffers(win);
//}
//
//// （追加）移動量表示ヘルパ（updateWindowPositionDiscrete / processGamepadDiscrete から使用）
//// ウィンドウ座標系は下方向が+なので表示は上下で符号反転し「上を正」にする
//static inline void printMoveAmounts(int offsetX, int offsetY) {
//	int dispVert = -offsetY;      // 上方向を正
//	int dispHorz = offsetX;       // 右方向を正
//	std::cout << "[移動量]上下:" << (dispVert >= 0 ? "+" : "") << dispVert << " px, 左右:"
//		<< (dispHorz >= 0 ? "+" : "") << dispHorz << " px\n";
//}
//
//// （追加）カメラ配置と baseOffset 再計算関数（カメラ配置ループの直前付近）
//static void recomputeCameraLayout(float gap,
//	glm::vec3* camPos,
//	std::vector<glm::vec2>& baseOffset,
//	int numLensX, int numLensY,
//	int halfX, int halfY,
//	float halfPitchX, float halfPitchY,
//	float focalLen, float imgPitch, int texW, int texH)
//{
//	const float scaleX = focalLen / imgPitch / (float)texW;
//	const float scaleY = focalLen / imgPitch / (float)texH;
//
//	for (int r = -halfY; r < halfY; ++r) {
//		int row = r + halfY;
//		float camPosY = (2.0f * (float)r + 1.0f) * halfPitchY;
//		for (int c = -halfX; c < halfX; ++c) {
//			int col = c + halfX;
//			float camPosX = (2.0f * (float)c + 1.0f) * halfPitchX;
//			if (gap != 0.0f) {
//				camPosX += (c >= 0 ? +0.5f : -0.5f) * gap;
//			}
//			int idx = row * numLensX + col;
//			camPos[idx] = glm::vec3(camPosX, camPosY, 0.0f);
//			baseOffset[idx] = glm::vec2(camPosX * scaleX, camPosY * scaleY);
//		}
//	}
//}