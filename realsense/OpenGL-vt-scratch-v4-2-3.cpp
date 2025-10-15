/*
	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
	v4-2:処理速度の高速化(テクスチャ付き平面オブジェクト化)
	v4-2-2:点群の3次元配列への集約を導入
	注意点:各方向のレンズ数、点群数ともに偶数を仮定！
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

#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#define GLFW_FALSE 0
#endif

// dGPU 要求 (Optimus / PowerXpress)
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace std;

// 定数(観察者側)
//------------------------------

const float MIN_OBSERVE_Z = 1.0f;

//------------------------------


// 定数(表示系側)
//------------------------------

const float DISPLAY_PX_PITCH = 13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);

const unsigned int NUM_LENS_W = 40;
const unsigned int NUM_LENS_H = 40;

const int HALF_NUM_LENS_W = NUM_LENS_W * 0.5f;
const int HALF_NUM_LENS_H = NUM_LENS_H * 0.5f;

const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * (float)NUM_LENS_H - 1.0f);

// レンズピッチを固定する場合(A-1)
const float LENS_PITCH_X = 0.00451f;
const float LENS_PITCH_Y = 0.00451f;
const float LENS_ARRAY_W = LENS_PITCH_X * NUM_LENS_W;
const float LENS_ARRAY_H = LENS_PITCH_Y * NUM_LENS_H;

//// レンズアレイ幅を固定する場合(A-2)
//const float LENS_ARRAY_W = 0.1804f;
//const float LENS_ARRAY_H = 0.1804f;
//const float LENS_PITCH_X = LENS_ARRAY_W / (float)NUM_LENS_W;
//const float LENS_PITCH_Y = LENS_ARRAY_H / (float)NUM_LENS_H;


const float HALF_LENS_PITCH_X = LENS_PITCH_X * 0.5f;
const float HALF_LENS_PITCH_Y = LENS_PITCH_Y * 0.5f;

//// 無限遠に向けた光線場再現の場合(B-1)
//const float ELEM_IMG_PITCH_X = LENS_PITCH_X;
//const float ELEM_IMG_PITCH_Y = LENS_PITCH_Y;

// 想定観察距離に向けた光線場再現の場合(B-2)
const float ELEM_IMG_PITCH_X = (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z * LENS_PITCH_X;
const float ELEM_IMG_PITCH_Y = (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z * LENS_PITCH_Y;

const unsigned int WIN_W = static_cast<unsigned int>(std::lround(NUM_LENS_W * ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH));
const unsigned int WIN_H = static_cast<unsigned int>(std::lround(NUM_LENS_H * ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH));
const unsigned int HALF_WIN_W = WIN_W / 2;
const unsigned int HALF_WIN_H = WIN_H / 2;

const float FLOAT_NUM_ELEM_IMG_PX_X = ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH;
const float FLOAT_NUM_ELEM_IMG_PX_Y = ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH;
const unsigned int NUM_ELEM_IMG_PX_X = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_X);
const unsigned int NUM_ELEM_IMG_PX_Y = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_Y);

const float DISPLAY_IMG_SIZE_X = WIN_W * DISPLAY_PX_PITCH;
const float DISPLAY_IMG_SIZE_Y = WIN_H * DISPLAY_PX_PITCH;

// 仮想カメラ設定
const float TAN_HALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
const float FOV_Y = atan(TAN_HALF_Y) * 2.0f;

//------------------------------


// 定数(被写体側)
//------------------------------

const float SUBJECT_Z = 1.0f;
const unsigned int NUM_SUBJECT_POINTS_X = 554;
const unsigned int NUM_SUBJECT_POINTS_Y = 554;
const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;
const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;
const float SUBJECT_SIZE_X = DISPLAY_IMG_SIZE_X * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
const float SUBJECT_SIZE_Y = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X);
const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y);
const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;

//------------------------------


// 定数(3次元配列側)
//------------------------------

const int N = 3;
const float Z_PLANE_IMG_PITCH = DISPLAY_PX_PITCH / (float)N;
const int Z_PLANE_IMG_PX_X = 100 * N;
const int Z_PLANE_IMG_PX_Y = 100 * N;
const int NUM_Z_PLANE = 60;
const float MIN_Z = 0.2f;
const float COEF_TRANS = ((float)NUM_Z_PLANE - 0.0f) * MIN_Z;
const float INV_COEF_TRANS = 1.0f / COEF_TRANS;
const int HALF_SEARCH_BOX_SIZE = (int)floor(N / 2);
//const int HALF_SEARCH_BOX_SIZE = 0;

//------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
static void printGLInfo(const char* tag);
static GLFWmonitor* getMonitorByIndex(int index);

int main(void) {

	cout << "MIN OBSERVE Z:" << MIN_OBSERVE_Z << endl;
	cout << "//----------------------------" << endl;
	cout << "DISPLAY PIXEL PITCH:" << DISPLAY_PX_PITCH << endl;
	cout << "NUM LENS WIDTH:" << NUM_LENS_W << endl;
	cout << "NUM LENS HEIGHT:" << NUM_LENS_H << endl;
	cout << "LENS ARRAY WIDTH:" << LENS_ARRAY_W << endl;
	cout << "LENS ARRAY HEIGHT:" << LENS_ARRAY_H << endl;
	cout << "LENS PITCH X:" << LENS_PITCH_X << endl;
	cout << "LENS PITCH Y:" << LENS_PITCH_Y << endl;
	cout << endl;
	cout << "ELEMENTAL IMAGE PITCH X:" << ELEM_IMG_PITCH_X << endl;
	cout << "ELEMENTAL IMAGE PITCH Y:" << ELEM_IMG_PITCH_Y << endl;
	cout << "FLOAT NUM ELEMENTAL IMAGE PX X:" << FLOAT_NUM_ELEM_IMG_PX_X << endl;
	cout << "FLOAT NUM ELEMENTAL IMAGE PX Y:" << FLOAT_NUM_ELEM_IMG_PX_Y << endl;
	cout << "NUM ELEMENTAL IMAGE PX X:" << NUM_ELEM_IMG_PX_X << endl;
	cout << "NUM ELEMENTAL IMAGE PX Y:" << NUM_ELEM_IMG_PX_Y << endl;
	cout << "DISPLAY IMG SIZE X:" << DISPLAY_IMG_SIZE_X << endl;
	cout << "DISPLAY IMG SIZE Y:" << DISPLAY_IMG_SIZE_Y << endl;
	cout << endl;
	cout << "FOCAL LENGTH:" << FOCAL_LENGTH << endl;
	cout << "TAN HALF Y:" << TAN_HALF_Y << endl;
	cout << "FOV Y:" << FOV_Y << endl;
	cout << "WINDOW WIDTH:" << WIN_W << endl;
	cout << "WINDOW HEIGHT:" << WIN_H << endl;
	cout << "//----------------------------" << endl;
	cout << "SUBJECT Z:" << SUBJECT_Z << endl;
	cout << "NUM SUBJECT POINTS X:" << NUM_SUBJECT_POINTS_X << endl;
	cout << "NUM SUBJECT POINTS Y:" << NUM_SUBJECT_POINTS_Y << endl;
	cout << "NUM POINTS:" << NUM_POINTS << endl;
	cout << "SUBJECT SIZE X:" << SUBJECT_SIZE_X << endl;
	cout << "SUBJECT SIZE Y:" << SUBJECT_SIZE_Y << endl;
	cout << "SUBJECT POINTS PITCH X:" << SUBJECT_POINTS_PITCH_X << endl;
	cout << "SUBJECT POINTS PITCH Y:" << SUBJECT_POINTS_PITCH_Y << endl;
	cout << "//----------------------------" << endl;
	cout << endl;


	// GLFWの初期設定
	//------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int targetMonitor = 0;
	GLFWmonitor* mon = getMonitorByIndex(targetMonitor);
	if (!mon) mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(mon);

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#ifdef GLFW_FLOATING
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	int offsetX = 0;
	int offsetY = 0;
	int winW = max((int)WIN_W, mode->width - offsetX);
	int winH = max((int)WIN_H, mode->height - offsetY);

	GLFWwindow* gridWin = glfwCreateWindow(winW, winH, "Grid Cameras", nullptr, nullptr);
	if (!gridWin) { std::fprintf(stderr, "Create window failed\n"); glfwTerminate(); return -1; }
	glfwMakeContextCurrent(gridWin);
	glfwSwapInterval(1);

	int mx = 0, my = 0;
	glfwGetMonitorPos(mon, &mx, &my);
	glfwSetWindowPos(gridWin, mx + offsetX, my + offsetY);
#ifdef GLFW_FLOATING
	glfwSetWindowAttrib(gridWin, GLFW_FLOATING, GLFW_TRUE);
#endif
	glfwShowWindow(gridWin);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::fprintf(stderr, "GLAD init failed\n"); return -1;
	}
	printGLInfo("Grid Cameras");

	//------------------------------


	// OpenGLの各種機能
	//------------------------------

	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//------------------------------


	// 頂点シェーダとフラグメントシェーダの指定
	Shader compose("shader-slices-compose.vert", "shader-slices-compose.frag");
	GLint locInvZ = glGetUniformLocation(compose.ID, "uInvZ");

	// 点群データの生成
	//------------------------------

	// 被写体画像読み込み
	cv::Mat image_input = cv::imread("./images/standard/mandrill.bmp");
	if (image_input.empty()) {
		std::cout << "画像を開くことができませんでした。\n";
		return -1;
	}
	cv::Mat resized_image;
	cv::resize(image_input, resized_image, cv::Size((int)NUM_SUBJECT_POINTS_Y, (int)NUM_SUBJECT_POINTS_X), 0, 0, cv::INTER_NEAREST);

	// 点群座標
	float* pointsPos = new float[NUM_POINTS * 3];
	for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
	{
		int row = r + HALF_NUM_SUBJECT_POINTS_Y;
		int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;

		for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
		{
			int col = c + HALF_NUM_SUBJECT_POINTS_X;
			int idx = (row * NUM_SUBJECT_POINTS_X + col) * 3;

			pointsPos[idx] = (2.0f * (float)c + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X;
			pointsPos[idx + 1] = (2.0f * (float)r + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y;
			pointsPos[idx + 2] = SUBJECT_Z;

		}
	}

	// 点群色情報
	unsigned char* pointsCol = new unsigned char[NUM_POINTS * 3];
	for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
	{
		int row = r + HALF_NUM_SUBJECT_POINTS_Y;
		int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;

		for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
		{
			int col = c + HALF_NUM_SUBJECT_POINTS_X;
			int idx = (row * NUM_SUBJECT_POINTS_X + col) * 3;

			pointsCol[idx] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[2];
			pointsCol[idx + 1] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[1];
			pointsCol[idx + 2] = (unsigned char)resized_image.at<cv::Vec3b>(reverseRow, col)[0];

		}
	}

	//cout << "SUBJECT POINT OFFSET X:" << points[0] << endl;
	//cout << "SUBJECT POINT OFFSET Y:" << points[1] << endl;

	//------------------------------


	// カメラ配置
	glm::vec3* camPos = new glm::vec3[NUM_LENS_W * NUM_LENS_H];
	for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r)
	{
		int row = r + HALF_NUM_LENS_H;
		float camPosY = (2.0f * (float)r + 1.0f) * HALF_LENS_PITCH_Y;

		for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c)
		{
			int col = c + HALF_NUM_LENS_W;
			float camPosX = (2.0f * (float)c + 1.0f) * HALF_LENS_PITCH_X;

			camPos[row * NUM_LENS_W + col] = glm::vec3(camPosX, camPosY, 0.0f);

			//cout << "camPos[" << row * NUM_LENS_W + col << "]:(" << camPos[row * NUM_LENS_W + col].x << "," << camPos[row * NUM_LENS_W + col].y << "," << camPos[row * NUM_LENS_W + col].z << ")" << endl;
		}
	}


	// テクスチャ配列の作成
	//------------------------------

	GLuint texSlices = 0; // 60層のテクスチャ配列
	{
		// 生成時のフィルタを線形に
		glGenTextures(1, &texSlices);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// 変更: 線形補間
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	// CPU→GPU 転送の一時バッファ（1層ぶんのRGBA）
	std::vector<unsigned char> rgbaPlane;
	rgbaPlane.resize(Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * 4);

	//------------------------------


	// VBO, VAOの作成
	//------------------------------

	GLuint quadVAO = 0, quadVBO = 0;
	{
		const float quad[] = {
			// pos    // uv
			-1.f, -1.f, 0.f, 0.f,
			 1.f, -1.f, 1.f, 0.f,
			 1.f,  1.f, 1.f, 1.f,
			-1.f, -1.f, 0.f, 0.f,
			 1.f,  1.f, 1.f, 1.f,
			-1.f,  1.f, 0.f, 1.f,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	//------------------------------


	unsigned int* red = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* green = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* blue = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	bool* alpha = new bool[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* numPoints = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];

	const int NZ = NUM_Z_PLANE;
	std::vector<float> invZ(NZ);
	for (int n = 0; n < NZ; ++n) {
		// z_t = 1/z を COEF_TRANS で量子化: bin中心を (n+0.5)/COEF_TRANS と近似
		invZ[n] = ((float)n + 0.5f) * INV_COEF_TRANS; // = (n+0.5)/COEF_TRANS
	}

	// baseOffset の前計算部分を置換
	const float texW = static_cast<float>(Z_PLANE_IMG_PX_X);
	const float texH = static_cast<float>(Z_PLANE_IMG_PX_Y);
	const float scaleX = FOCAL_LENGTH / Z_PLANE_IMG_PITCH / texW; // = (F/img_pitch)/TexW
	const float scaleY = FOCAL_LENGTH / Z_PLANE_IMG_PITCH / texH; // = (F/img_pitch)/TexH

	std::vector<glm::vec2> baseOffset(NUM_LENS_W * NUM_LENS_H);
	for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r) {
		int row = r + HALF_NUM_LENS_H;
		for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c) {
			int col = c + HALF_NUM_LENS_W;
			int idx = row * NUM_LENS_W + col;
			float cx = camPos[idx].x; // s に相当
			float cy = camPos[idx].y; // t に相当
			baseOffset[idx] = glm::vec2(cx * scaleX, cy * scaleY); // 符号は座標系に依存。見え方に応じて ± を調整
		}
	}

	const float uvScaleX = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_X / (float)Z_PLANE_IMG_PX_X; // = N * NUM_ELEM_IMG_PX_X / Z_PLANE_IMG_PX_X
	const float uvScaleY = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_Y / (float)Z_PLANE_IMG_PX_Y; // = N * NUM_ELEM_IMG_PX_Y / Z_PLANE_IMG_PX_Y

	// フレーム処理
	//------------------------------

	glfwSetFramebufferSizeCallback(gridWin, framebuffer_size_callback);

	long long sum_time = 0;
	int numFrame = 0;
	const float invZo = 1.0f / MIN_OBSERVE_Z;

	while (!glfwWindowShouldClose(gridWin))
	{
		// 測定開始時刻を記録
		auto start = std::chrono::high_resolution_clock::now();

		processInput(gridWin);

		// 1) 各層バッファの初期化
		for (int i = 0; i < NUM_Z_PLANE; i++) {
			for (int j = 0; j < Z_PLANE_IMG_PX_Y; j++) {
				for (int k = 0; k < Z_PLANE_IMG_PX_X; k++) {
					const int idx = (i * Z_PLANE_IMG_PX_Y + j) * Z_PLANE_IMG_PX_X + k;
					red[idx] = 0u;
					green[idx] = 0u;
					blue[idx] = 0u;
					alpha[idx] = false;
					numPoints[idx] = 0u;
				}
			}
		}

		// 2) 点群のビニング（幾何変換 → 量子化 → 近傍書き込み）
		for (int k = 0; k < NUM_POINTS * 3; k += 3) {

			float tmp_pcd_x = pointsPos[k + 0];
			float tmp_pcd_y = pointsPos[k + 1];
			float tmp_pcd_z = pointsPos[k + 2];
			unsigned char tmp_pcd_b = pointsCol[k + 2];
			unsigned char tmp_pcd_g = pointsCol[k + 1];
			unsigned char tmp_pcd_r = pointsCol[k + 0];

			float tmp_zt = 1.0f / tmp_pcd_z;
			float tmp_xt = tmp_pcd_x * tmp_zt;
			float tmp_yt = tmp_pcd_y * tmp_zt;

			int tmp_nx = static_cast<int>(std::floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_xt + 0.5f) + Z_PLANE_IMG_PX_X * 0.5f);
			int tmp_ny = static_cast<int>(std::floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_yt + 0.5f) + Z_PLANE_IMG_PX_Y * 0.5f);
			int tmp_nz = static_cast<int>(std::floor(COEF_TRANS * tmp_zt));

			if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
				for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; m++) {
					for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; n++) {
						int x = tmp_nx + n;
						int y = tmp_ny + m;
						if (0 <= x && x < Z_PLANE_IMG_PX_X && 0 <= y && y < Z_PLANE_IMG_PX_Y) {
							const int idx = (tmp_nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
							blue[idx] += static_cast<unsigned int>(tmp_pcd_b);
							green[idx] += static_cast<unsigned int>(tmp_pcd_g);
							red[idx] += static_cast<unsigned int>(tmp_pcd_r);
							alpha[idx] = true;
							numPoints[idx]++;
						}
					}
				}
			}
		}

		// 3) 各画素の平均化
		for (int i = 0; i < NUM_Z_PLANE; i++) {
			for (int j = 0; j < Z_PLANE_IMG_PX_Y; j++) {
				for (int k = 0; k < Z_PLANE_IMG_PX_X; k++) {
					const int idx = (i * Z_PLANE_IMG_PX_Y + j) * Z_PLANE_IMG_PX_X + k;
					if (alpha[idx] && numPoints[idx] > 0u) {
						// 四捨五入で丸めたい場合は + numPoints[idx]/2 を加えてから割る
						red[idx] = (red[idx] + numPoints[idx] * 0.5f) / numPoints[idx];
						green[idx] = (green[idx] + numPoints[idx] * 0.5f) / numPoints[idx];
						blue[idx] = (blue[idx] + numPoints[idx] * 0.5f) / numPoints[idx];
					}
				}
			}
		}

		// 4) 各層をテクスチャ配列へアップロード（RGBA8, A=存在フラグ）
		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (int nz = 0; nz < NUM_Z_PLANE; ++nz) {
			for (int y = 0; y < Z_PLANE_IMG_PX_Y; ++y) {
				for (int x = 0; x < Z_PLANE_IMG_PX_X; ++x) {
					const int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
					const int out = (y * Z_PLANE_IMG_PX_X + x) * 4;

					// 0..255 にクランプして詰める
					rgbaPlane[out + 0] = static_cast<unsigned char>(std::min(red[idx], 255u));
					rgbaPlane[out + 1] = static_cast<unsigned char>(std::min(green[idx], 255u));
					rgbaPlane[out + 2] = static_cast<unsigned char>(std::min(blue[idx], 255u));
					rgbaPlane[out + 3] = alpha[idx] ? 255 : 0;
				}
			}

			glTexSubImage3D(
				GL_TEXTURE_2D_ARRAY, 0,
				0, 0, nz,
				Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, 1,
				GL_RGBA, GL_UNSIGNED_BYTE, rgbaPlane.data()
			);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		// 5) 合成パス（各タイル=1ドロー、シェーダ内で60層ループ＋早期終了）
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		compose.use();
		compose.setInt("uSlices", 0);
		compose.setInt("uNZ", NZ);
		compose.setVec2("uUvScale", glm::vec2(uvScaleX, uvScaleY));
		compose.setVec2("uTexelSize", glm::vec2(1.0f / Z_PLANE_IMG_PX_X, 1.0f / Z_PLANE_IMG_PX_Y));
		glUniform1fv(locInvZ, NZ, invZ.data());

		// invZ はフレーム毎に一度設定（UBOがない前提）
		glUniform1fv(locInvZ, NZ, invZ.data());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);

		glEnable(GL_SCISSOR_TEST);
		glBindVertexArray(quadVAO);

		for (int r = -HALF_NUM_LENS_H; r < HALF_NUM_LENS_H; ++r) {
			int row = r + HALF_NUM_LENS_H;
			for (int c = -HALF_NUM_LENS_W; c < HALF_NUM_LENS_W; ++c) {
				int col = c + HALF_NUM_LENS_W;
				int idx = row * NUM_LENS_W + col;

				int vx = lround(col * FLOAT_NUM_ELEM_IMG_PX_X);
				int vy = lround(row * FLOAT_NUM_ELEM_IMG_PX_Y);

				glViewport(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);
				glScissor(vx, vy, NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y);

				compose.setVec2("uBaseOffset", baseOffset[idx]);
				glm::vec2 frustumUVShift = baseOffset[idx] * invZo;
				compose.setVec2("uFrustumUVShift", frustumUVShift);

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		glBindVertexArray(0);
		glDisable(GL_SCISSOR_TEST);
		glUseProgram(0);

		glfwSwapBuffers(gridWin);
		glfwPollEvents();

		// 測定終了時刻を記録
		auto end = std::chrono::high_resolution_clock::now();

		// 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		sum_time += duration.count();
		numFrame++;

	}

	cout << "フレーム数:" << numFrame << endl;
	cout << "平均実行時間: " << sum_time / numFrame << " ms" << endl;

	//------------------------------


	// 表示画像の保存
	//------------------------------

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
	stream << "D:/ForStudy/reconstruction/OpenGL-scratch-wideview-v4-2-3/OpenGL-scratch-wideview-v4-2-3-mandrill_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << ".png";
	std::string outPath = stream.str();

	cv::imwrite(outPath, img);

	//------------------------------


	// 後処理
	//------------------------------

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteTextures(1, &texSlices);

	delete[] camPos;
	delete[] pointsPos;
	delete[] pointsCol;
	delete[] red;
	delete[] green;
	delete[] blue;
	delete[] alpha;
	delete[] numPoints;

	glfwTerminate();

	//------------------------------


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

static void printGLInfo(const char* tag) {
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::printf("[%s] GL_VENDOR   = %s\n", tag, vendor ? (const char*)vendor : "(null)");
	std::printf("[%s] GL_RENDERER = %s\n", tag, renderer ? (const char*)renderer : "(null)");
	std::printf("[%s] GL_VERSION  = %s\n", tag, version ? (const char*)version : "(null)");
	std::printf("[%s] GLSL        = %s\n", tag, glsl ? (const char*)glsl : "(null)");
}

static GLFWmonitor* getMonitorByIndex(int index) {
	int count = 0;
	GLFWmonitor** mons = glfwGetMonitors(&count);
	if (count <= 0) return nullptr;
	if (index < 0 || index >= count) return glfwGetPrimaryMonitor();
	return mons[index];
}
