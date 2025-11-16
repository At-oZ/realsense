/*
	PCSJ 2025 向けコード
	左右2ウィンドウ分割描画 + コントローラ操作 + RGB-D(RealSense)点群を体積化して投影
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>

#include <opencv2/opencv.hpp>
#include <librealsense2/rs.hpp>

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

#ifndef NOMINMAX
#define NOMINMAX
#endif
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

// ディスプレイの画素ピッチ
const float DISPLAY_PX_PITCH = 13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);

// レンズ数
const unsigned int NUM_LENS_X = 20;
const unsigned int NUM_LENS_Y = 16;

// レンズ数の半分
const int HALF_NUM_LENS_X = NUM_LENS_X * 0.5f;
const int HALF_NUM_LENS_Y = NUM_LENS_Y * 0.5f;

// レンズの焦点距離
const float FOCAL_LENGTH = 0.0068f;

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


/* レンズピッチ至上主義 */
// レンズアレイ幅
const float LENS_ARRAY_LONG_WIDTH = 0.2869f;
const float LENS_ARRAY_SHORT_WIDTH = 0.17925f;

const float LENS_PITCH_X = LENS_ARRAY_SHORT_WIDTH / NUM_LENS_X * 2.0f;
const float LENS_PITCH_Y = LENS_ARRAY_LONG_WIDTH / NUM_LENS_Y;

/* 要素画像の間隔 */
//// 無限遠に向けた光線場再現の場合(A-1)
// const string VIEW_MODE = "normal";
//const float ELEM_IMG_PITCH_X = LENS_PITCH_X;
//const float ELEM_IMG_PITCH_Y = LENS_PITCH_Y;

// 想定観察距離に向けた光線場再現の場合(A-2)
const string VIEW_MODE = "wideview";
const float ELEM_IMG_PITCH_X = LENS_PITCH_X * (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z;
const float ELEM_IMG_PITCH_Y = LENS_PITCH_Y * (FOCAL_LENGTH + MIN_OBSERVE_Z) / MIN_OBSERVE_Z;

// 要素画像の解像度の浮動小数点数
const float FLOAT_NUM_ELEM_IMG_PX_X = ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH;
const float FLOAT_NUM_ELEM_IMG_PX_Y = ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH;

// 要素画像の解像度
const unsigned int NUM_ELEM_IMG_PX_X = (int)FLOAT_NUM_ELEM_IMG_PX_X;
const unsigned int NUM_ELEM_IMG_PX_Y = (int)FLOAT_NUM_ELEM_IMG_PX_Y;


// レンズピッチの半分
const float HALF_LENS_PITCH_X = LENS_PITCH_X / 2.0f;
const float HALF_LENS_PITCH_Y = LENS_PITCH_Y / 2.0f;

// 表示画像の解像度
const unsigned int NUM_DISPLAY_IMG_PX_X = NUM_ELEM_IMG_PX_X * NUM_LENS_X;
const unsigned int NUM_DISPLAY_IMG_PX_Y = NUM_ELEM_IMG_PX_Y * NUM_LENS_Y;

// 表示画像のサイズ
const float DISPLAY_IMG_SIZE_X = NUM_DISPLAY_IMG_PX_X * DISPLAY_PX_PITCH;
const float DISPLAY_IMG_SIZE_Y = NUM_DISPLAY_IMG_PX_Y * DISPLAY_PX_PITCH;

// 仮想カメラ設定
const float TAN_YALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
const float FOV_Y = atan(TAN_YALF_Y) * 2.0f;

//------------------------------

// 定数(3次元配列側)
//------------------------------

const int BOX_DETAIL_N = 3;
const float Z_PLANE_IMG_PITCH = DISPLAY_PX_PITCH / (float)BOX_DETAIL_N;
const int Z_PLANE_IMG_PX_X = 600;
const int Z_PLANE_IMG_PX_Y = 480;
const int NUM_Z_PLANE = 60;
const float BOX_MIN_Z = 0.2f;
const float COEF_TRANS = ((float)NUM_Z_PLANE - 0.0f) * BOX_MIN_Z;
const float INV_COEF_TRANS = 1.0f / COEF_TRANS;
const int HALF_SEARCH_BOX_SIZE = (int)floor(BOX_DETAIL_N / 2);
const float F_OVER_Z_PLANE_PITCH = FOCAL_LENGTH / Z_PLANE_IMG_PITCH;
const int HALF_Z_PLANE_IMG_PX_X = Z_PLANE_IMG_PX_X / 2;
const int HALF_Z_PLANE_IMG_PX_Y = Z_PLANE_IMG_PX_Y / 2;

//------------------------------


// 定数(RGB-Dカメラ側)
//------------------------------

const int RGBD_IMG_WIDTH = 640;
const int RGBD_IMG_HEIGHT = 480;
const int FPS = 30;
const int NUM_POINTS = RGBD_IMG_WIDTH * RGBD_IMG_HEIGHT;
const float INV_RGBD_IMG_WIDTH = 1.0f / (float)RGBD_IMG_WIDTH;
constexpr float DISTANCE_CAMERA_DISPLAY = 0.1258f;
constexpr float DIFF_HEIGHT_CAMERA_DISPLAY = -0.06f;

//------------------------------


// 定数(そのほか)
//------------------------------

const int NUM_THREADS = 30;

//------------------------------

// centerDistance ランタイム調整関連（ウィンドウ間ギャップ）
const float INITIAL_CENTER_DISTANCE = 0.067f; // 初期ギャップ(m)
float centerDistance = INITIAL_CENTER_DISTANCE; // 変更可能ギャップ(m)
const float CENTER_DISTANCE_STEP_FINE = 0.001f;
const float CENTER_DISTANCE_STEP_COARSE = 0.005f;
bool gapDirty = true; // 初回再計算要求

// 焦点距離動的調整用（初期値は既存定数 FOCAL_LENGTH）
float focalLength = FOCAL_LENGTH;
const float FOCAL_STEP_FINE = 0.0001f;
const float FOCAL_STEP_COARSE = 0.0005f;
const float FOCAL_MIN = 0.002f;
const float FOCAL_MAX = 0.050f;
bool focalDirty = true; // 初期再計算要求

// ==== 追加1: 高さ調整用ランタイム変数（DIFF_HEIGHT_CAMERA_DISPLAY 定義直後あたり） ====
float diffHeight = DIFF_HEIGHT_CAMERA_DISPLAY;          // カメラ高さ差の現在値 (m)
const float HEIGHT_STEP_FINE = 0.001f;                // 微調整ステップ
const float HEIGHT_STEP_COARSE = 0.005f;                // LB/RB 同時時の粗ステップ
const float HEIGHT_MIN = -0.200f;
const float HEIGHT_MAX = 0.200f;
bool heightDirty = false; // 必要なら再計算トリガ（現在は点群変換のみなので描画側再計算不要）

// ==== 追加A: 水平位置調整用ランタイム変数（diffHeight 定義の直後に追記） ====
float diffX = 0.0f;                          // 点群の水平オフセット (m)
const float X_STEP_FINE = 0.001f;           // 微調整ステップ
const float X_STEP_COARSE = 0.005f;           // LB/RB 同時時の粗ステップ
const float X_MIN = -0.200f;
const float X_MAX = 0.200f;
bool xDirty = false;                          // ログ用途（再計算は不要）

// ==== 追加: 左右独立用パラメータ（既存 diffX/diffHeight/focalLength 定義直後） ====
// 従来の単一 diffX/diffHeight/focalLength は以後使用しない（残しても可だが参照を避ける）
float focalLengthLeft = FOCAL_LENGTH;
float focalLengthRight = FOCAL_LENGTH;
float diffHeightLeft = diffHeight;
float diffHeightRight = diffHeight;
float diffXLeft = diffX;
float diffXRight = diffX;

bool focalDirtyLeft = true;
bool focalDirtyRight = true;
bool heightDirtyLeft = false;
bool heightDirtyRight = false;
bool xDirtyLeft = false;
bool xDirtyRight = false;

enum class OperationMode { WindowMove, CenterDistanceAdjust, PointCloudShiftAdjust, FocalLengthAdjust };
OperationMode controlMode = OperationMode::WindowMove;

// ウィンドウ移動状態
struct WindowMoveState {
	int prevUp = GLFW_RELEASE;
	int prevDown = GLFW_RELEASE;
	int prevLeft = GLFW_RELEASE;
	int prevRight = GLFW_RELEASE;
};

// Gamepad用エッジ検出
struct GamepadEdgeState { unsigned char up = 0, down = 0, left = 0, right = 0, x = 0, back = 0, y = 0, a = 0; };

// プロトタイプ
static void printGLInfo(const char* tag);
static GLFWmonitor* getMonitorByIndex(int index);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints);
void averageSlices(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, const unsigned int* numPoints);
void packSlicesRGBA(int start, int end, const unsigned int* red, const unsigned int* green, const unsigned int* blue, const unsigned int* numPoints, unsigned char* rgba3D);

static void updateWindowPositionDiscrete(GLFWwindow* window, int& offsetX, int& offsetY, int monX, int monY, int upKey, int downKey, int leftKey, int rightKey, WindowMoveState& state);
static void toggleFocusWithKey(GLFWwindow* winLeft, GLFWwindow* winRight);
static void focusWindowPortable(GLFWwindow* w);

static void logAllJoysticksOnce();
static int  selectPrimaryGamepad();
static void logJoystickButtonsAxes(int jid);
// ==== 変更: processGamepadDiscrete プロトタイプを左右独立値に対応 ====
// 旧プロトタイプ差し替え
static void processGamepadDiscrete(GLFWwindow* winLeft, GLFWwindow* winRight,
	int& offsetXLeft, int& offsetYLeft, int& offsetXRight, int& offsetYRight,
	int monX, int monY, GamepadEdgeState& gp, int gamepadIndex,
	float& centerDistance, bool& gapDirty, OperationMode& controlMode,
	float& focalLengthLeft, bool& focalDirtyLeft,
	float& focalLengthRight, bool& focalDirtyRight,
	float& diffHeightLeft, bool& heightDirtyLeft,
	float& diffHeightRight, bool& heightDirtyRight,
	float& diffXLeft, bool& xDirtyLeft,
	float& diffXRight, bool& xDirtyRight);

// タイル描画（コンテキスト毎に呼ぶ）
static void drawTiles(
	GLFWwindow* win, Shader& compose,
	const glm::vec2* baseOffset, int lensColsStart, int lensColsCount,
	int lensRowsStart, int lensRowsCount,
	int tileW, int tileH,
	int halfCols, int halfRows,
	int nz, GLint locInvZ, const float* invZ, float uvScaleX, float uvScaleY,
	int texW, int texH, float invZo, GLuint texSlices, GLuint quadVAO);

static inline void printMoveAmounts(int offsetX, int offsetY);

// ギャップ適用版のカメラ配置と baseOffset 再計算
static void recomputeCameraLayout(float gap,
	glm::vec3* camPos,
	std::vector<glm::vec2>& baseOffset,
	int numLensX, int numLensY,
	int halfX, int halfY,
	float halfPitchX, float halfPitchY,
	float focalLen, float imgPitch, int texW, int texH);

// ==== 変更: CSV ロード/セーブ関数プロトタイプ差し替え（左右独立値対応） ====
static bool loadSettingsCSV(const std::string& path,
	int& offsetXLeft, int& offsetYLeft, int& offsetXRight, int& offsetYRight,
	float& diffXLeft, float& diffHeightLeft, float& focalLengthLeft,
	float& diffXRight, float& diffHeightRight, float& focalLengthRight,
	float& centerDistance);

static void saveSettingsCSV(const std::string& path,
	int offsetXLeft, int offsetYLeft, int offsetXRight, int offsetYRight,
	float diffXLeft, float diffHeightLeft, float focalLengthLeft,
	float diffXRight, float diffHeightRight, float focalLengthRight,
	float centerDistance);

// エントリ
int main() {

	std::cout << "MIN OBSERVE Z:" << MIN_OBSERVE_Z << std::endl;
	std::cout << "//----------------------------" << std::endl;
	std::cout << "DISPLAY PIXEL PITCH:" << DISPLAY_PX_PITCH << std::endl;
	std::cout << "NUM LENS WIDTH:" << NUM_LENS_X << std::endl;
	std::cout << "NUM LENS HEIGHT:" << NUM_LENS_Y << std::endl;
	std::cout << "LENS PITCH X:" << LENS_PITCH_X << std::endl;
	std::cout << "LENS PITCH Y:" << LENS_PITCH_Y << std::endl;
	std::cout << std::endl;
	std::cout << "ELEMENTAL IMAGE PITCH X:" << ELEM_IMG_PITCH_X << std::endl;
	std::cout << "ELEMENTAL IMAGE PITCH Y:" << ELEM_IMG_PITCH_Y << std::endl;
	std::cout << "FLOAT NUM ELEMENTAL IMAGE PX X:" << FLOAT_NUM_ELEM_IMG_PX_X << std::endl;
	std::cout << "FLOAT NUM ELEMENTAL IMAGE PX Y:" << FLOAT_NUM_ELEM_IMG_PX_Y << std::endl;
	std::cout << "NUM ELEMENTAL IMAGE PX X:" << NUM_ELEM_IMG_PX_X << std::endl;
	std::cout << "NUM ELEMENTAL IMAGE PX Y:" << NUM_ELEM_IMG_PX_Y << std::endl;
	std::cout << "DISPLAY IMG SIZE X:" << DISPLAY_IMG_SIZE_X << std::endl;
	std::cout << "DISPLAY IMG SIZE Y:" << DISPLAY_IMG_SIZE_Y << std::endl;
	std::cout << std::endl;
	std::cout << "FOCAL LENGTH:" << FOCAL_LENGTH << std::endl;
	std::cout << "TAN HALF Y:" << TAN_YALF_Y << std::endl;
	std::cout << "FOV Y:" << FOV_Y << std::endl;
	std::cout << "WINDOW WIDTH:" << NUM_DISPLAY_IMG_PX_X << std::endl;
	std::cout << "WINDOW HEIGHT:" << NUM_DISPLAY_IMG_PX_Y << std::endl;
	std::cout << "//----------------------------" << std::endl;
	std::cout << std::endl;

	// GLFWの初期設定
	//------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int targetMonitor = 0;
	GLFWmonitor* mon = getMonitorByIndex(targetMonitor);
	if (!mon) mon = glfwGetPrimaryMonitor();

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#ifdef GLFW_FLOATING
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	// 左右分割
	const int LEFT_COLS = NUM_LENS_X / 2;
	const int RIGHT_COLS = NUM_LENS_X - LEFT_COLS;
	const int leftWinW = LEFT_COLS * NUM_ELEM_IMG_PX_X;
	const int rightWinW = RIGHT_COLS * NUM_ELEM_IMG_PX_X;
	const int winH = NUM_LENS_Y * NUM_ELEM_IMG_PX_Y;

	// オフセット
	int offsetXLeft = 0, offsetYLeft = 0;
	int offsetXRight = leftWinW + offsetXLeft, offsetYRight = 0;

	// ==== 変更: CSVロード呼び出し部（main の初期オフセット設定直後） ====
	// 旧 loadSettingsCSV 呼び出しを置換
	if (loadSettingsCSV("settings.csv",
		offsetXLeft, offsetYLeft, offsetXRight, offsetYRight,
		diffXLeft, diffHeightLeft, focalLengthLeft,
		diffXRight, diffHeightRight, focalLengthRight,
		centerDistance))
	{
		std::cout << "[Settings] CSV読み込み成功: focal(L)=" << focalLengthLeft
			<< " focal(R)=" << focalLengthRight
			<< " centerDistance=" << centerDistance
			<< " diffX(L)=" << diffXLeft << " diffHeight(L)=" << diffHeightLeft
			<< " diffX(R)=" << diffXRight << " diffHeight(R)=" << diffHeightRight << "\n";
		gapDirty = true;
		focalDirtyLeft = true;
		focalDirtyRight = true;
	}

	// 左ウィンドウ
	GLFWwindow* winLeft = glfwCreateWindow(leftWinW, winH, "Grid Cameras Left", nullptr, nullptr);
	if (!winLeft) { std::fprintf(stderr, "Create left window failed\n"); glfwTerminate(); return -1; }
	glfwMakeContextCurrent(winLeft);
	glfwSwapInterval(1);

	int mx = 0, my = 0;
	glfwGetMonitorPos(mon, &mx, &my);
	glfwSetWindowPos(winLeft, mx + offsetXLeft, my + offsetYLeft);
#ifdef GLFW_FLOATING
	glfwSetWindowAttrib(winLeft, GLFW_FLOATING, GLFW_TRUE);
#endif
	glfwShowWindow(winLeft);

	// GLAD (一度だけ)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::fprintf(stderr, "GLAD init failed\n"); return -1;
	}
	printGLInfo("LeftWindow");

	// 右ウィンドウ（共有コンテキスト）
	GLFWwindow* winRight = glfwCreateWindow(rightWinW, winH, "Grid Cameras Right", nullptr, winLeft);
	if (!winRight) { std::fprintf(stderr, "Create right window failed\n"); glfwTerminate(); return -1; }
	glfwSetWindowPos(winRight, mx + offsetXRight, my + offsetYRight);
#ifdef GLFW_FLOATING
	glfwSetWindowAttrib(winRight, GLFW_FLOATING, GLFW_TRUE);
#endif
	glfwShowWindow(winRight);

	//------------------------------

	// Gamepad 接続待機
	//------------------------------

	std::cout << "ゲームパッド接続待機中...\n";
	int gamepadIndex = -1;
	while (gamepadIndex < 0 && !glfwWindowShouldClose(winLeft) && !glfwWindowShouldClose(winRight)) {
		for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
			if (glfwJoystickPresent(jid) == GLFW_TRUE) {
				gamepadIndex = jid;
				break;
			}
		}
		if (gamepadIndex >= 0) break;
		glfwPollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
	if (gamepadIndex < 0) {
		std::cout << "ゲームパッド未接続のまま継続します（後で接続すれば自動再検出）。\n";
	}
	else {
		const char* joyName = glfwGetJoystickName(gamepadIndex);
		std::cout << "ゲームパッド検出: id=" << gamepadIndex
			<< " name='" << (joyName ? joyName : "unknown") << "' "
			<< (glfwJoystickIsGamepad(gamepadIndex) ? "[mapped]" : "[raw]") << "\n";
	}

	//------------------------------

	// OpenGLの各種機能
	//------------------------------

	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//------------------------------

	// Shader
	Shader compose("shader-slices-compose.vert", "shader-slices-compose.frag");
	GLint locInvZLeft = glGetUniformLocation(compose.ID, "uInvZ");
	glfwMakeContextCurrent(winRight);
	GLint locInvZRight = glGetUniformLocation(compose.ID, "uInvZ");
	glfwMakeContextCurrent(winLeft);

	// カメラ配置 + baseOffset（ギャップ対応）
	glm::vec3* camPos = new glm::vec3[NUM_LENS_X * NUM_LENS_Y];
	std::vector<glm::vec2> baseOffset(NUM_LENS_X * NUM_LENS_Y);
	recomputeCameraLayout(centerDistance,
		camPos,
		baseOffset,
		NUM_LENS_X, NUM_LENS_Y,
		HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
		HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y,
		FOCAL_LENGTH, Z_PLANE_IMG_PITCH,
		Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);

	// ==== 追加: 左右用 baseOffset / camPos の確保（既存単一の直後に追記） ====
	glm::vec3* camPosLeft = new glm::vec3[NUM_LENS_X * NUM_LENS_Y];
	glm::vec3* camPosRight = new glm::vec3[NUM_LENS_X * NUM_LENS_Y];
	std::vector<glm::vec2> baseOffsetLeft(NUM_LENS_X * NUM_LENS_Y);
	std::vector<glm::vec2> baseOffsetRight(NUM_LENS_X * NUM_LENS_Y);

	recomputeCameraLayout(centerDistance, camPosLeft, baseOffsetLeft,
		NUM_LENS_X, NUM_LENS_Y, HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
		HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y, focalLengthLeft, Z_PLANE_IMG_PITCH,
		Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);
	recomputeCameraLayout(centerDistance, camPosRight, baseOffsetRight,
		NUM_LENS_X, NUM_LENS_Y, HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
		HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y, focalLengthRight, Z_PLANE_IMG_PITCH,
		Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);

	// テクスチャ配列の作成
	//------------------------------

	// ==== 追加: 左右別テクスチャ / 集計バッファ（既存バッファ生成部差し替え） ====
	// 旧 texSlices / red/green/blue/numPoints を左右に分離
	GLuint texSlicesLeft = 0, texSlicesRight = 0;
	glGenTextures(1, &texSlicesLeft);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texSlicesLeft);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &texSlicesRight);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texSlicesRight);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	std::vector<unsigned char> rgba3DLeft(Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE * 4);
	std::vector<unsigned char> rgba3DRight(Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE * 4);

	unsigned int* redLeft = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* greenLeft = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* blueLeft = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* numPointsLeft = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];

	unsigned int* redRight = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* greenRight = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* blueRight = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* numPointsRight = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];

	// CPU→GPU 転送バッファ
	std::vector<unsigned char> rgba3D(Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE * 4);

	// VAO/VBO（左）
	//------------------------------

	GLuint quadVAO = 0, quadVBO = 0;
	{
		const float quad[] = {
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

	// VAO/VBO（右）※VAOは共有されないため右で作成
	GLuint quadVAORight = 0, quadVBORight = 0;
	glfwMakeContextCurrent(winRight);
	{
		const float quad[] = {
			-1.f, -1.f, 0.f, 0.f,
			 1.f, -1.f, 1.f, 0.f,
			 1.f,  1.f, 1.f, 1.f,
			-1.f, -1.f, 0.f, 0.f,
			 1.f,  1.f, 1.f, 1.f,
			-1.f,  1.f, 0.f, 1.f,
		};
		glGenVertexArrays(1, &quadVAORight);
		glGenBuffers(1, &quadVBORight);
		glBindVertexArray(quadVAORight);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBORight);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}
	glfwMakeContextCurrent(winLeft);

	//------------------------------

	// invZ 配列
	const int NZ = NUM_Z_PLANE;
	std::vector<float> invZ(NZ);
	for (int n = 0; n < NZ; ++n) invZ[n] = ((float)n) * INV_COEF_TRANS;

	// baseOffset は上の recomputeCameraLayout で算出済み
	const float uvScaleX = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_X / (float)Z_PLANE_IMG_PX_X;
	const float uvScaleY = (DISPLAY_PX_PITCH / Z_PLANE_IMG_PITCH) * FLOAT_NUM_ELEM_IMG_PX_Y / (float)Z_PLANE_IMG_PX_Y;

	// スライス集計バッファ
	unsigned int* red = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* green = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* blue = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];
	unsigned int* numPoints = new unsigned int[Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * NUM_Z_PLANE];

	// RealSense
	//------------------------------

	rs2::config config;
	config.enable_stream(RS2_STREAM_COLOR, RGBD_IMG_WIDTH, RGBD_IMG_HEIGHT, RS2_FORMAT_BGR8, FPS);
	config.enable_stream(RS2_STREAM_DEPTH, RGBD_IMG_WIDTH, RGBD_IMG_HEIGHT, RS2_FORMAT_Z16, FPS);

	rs2::pipeline pipe;
	pipe.start(config);

	auto profile = pipe.get_active_profile();
	auto sensors = profile.get_device().query_sensors();
	if (sensors.size() >= 2) {
		auto colorSensors = sensors[1];
		auto irSensor = sensors[0];
		colorSensors.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
		colorSensors.set_option(RS2_OPTION_EXPOSURE, 150);
		colorSensors.set_option(RS2_OPTION_GAIN, 64);
		irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
		irSensor.set_option(RS2_OPTION_LASER_POWER, 360);
	}
	rs2::pointcloud pc;
	rs2::points points;
	rs2::align align_to_color(RS2_STREAM_COLOR);

	//------------------------------

	// コールバック・状態
	glfwSetFramebufferSizeCallback(winLeft, framebuffer_size_callback);
	glfwSetFramebufferSizeCallback(winRight, framebuffer_size_callback);

	WindowMoveState moveLeftState, moveRightState;
	GamepadEdgeState gpState;

	const float invZo = 1.0f / MIN_OBSERVE_Z;

	double lastTime = glfwGetTime();
	double lastLog = 0.0;

	// （追加）累積用タイミング変数（メインループ直前、while の前あたり）
	double sum_gpu_time_ms = 0.0;
	double sum_cpu_time_ms = 0.0;
	int    frameCount = 0;

	// フレーム処理
	//------------------------------

	std::cout << "[Mode] ウィンドウ移動モード (フォーカス=左ウィンドウ)" << std::endl;

	while (!glfwWindowShouldClose(winLeft) && !glfwWindowShouldClose(winRight)) {

		// ==== 計測開始 ====
		auto cpuStart = std::chrono::high_resolution_clock::now();
		// GPUクエリは左コンテキストで取得（共有リソース使用の代表とする）
		glfwMakeContextCurrent(winLeft);
		GLuint timeQuery = 0;
		glGenQueries(1, &timeQuery);
		glBeginQuery(GL_TIME_ELAPSED, timeQuery);

		processInput(winLeft);
		processInput(winRight);

		double currentTime = glfwGetTime();
		(void)currentTime;

		// キー: F でフォーカス切替
		toggleFocusWithKey(winLeft, winRight);

		// ==== 変更: main ループ内 processGamepadDiscrete 呼び出し ====
		// 引数増加版に置換
		processGamepadDiscrete(winLeft, winRight,
			offsetXLeft, offsetYLeft, offsetXRight, offsetYRight,
			mx, my, gpState, gamepadIndex,
			centerDistance, gapDirty, controlMode,
			focalLengthLeft, focalDirtyLeft,
			focalLengthRight, focalDirtyRight,
			diffHeightLeft, heightDirtyLeft,
			diffHeightRight, heightDirtyRight,
			diffXLeft, xDirtyLeft,
			diffXRight, xDirtyRight);

		// ==== 追加: 左右別再計算（gapDirty または個別 focalDirty） ====
		// 旧 if (gapDirty || focalDirty) ブロック差し替え
		if (gapDirty || focalDirtyLeft) {
			recomputeCameraLayout(centerDistance,
				camPosLeft, baseOffsetLeft,
				NUM_LENS_X, NUM_LENS_Y, HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
				HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y,
				focalLengthLeft, Z_PLANE_IMG_PITCH,
				Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);
			focalDirtyLeft = false;
		}
		if (gapDirty || focalDirtyRight) {
			recomputeCameraLayout(centerDistance,
				camPosRight, baseOffsetRight,
				NUM_LENS_X, NUM_LENS_Y, HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
				HALF_LENS_PITCH_X, HALF_LENS_PITCH_Y,
				focalLengthRight, Z_PLANE_IMG_PITCH,
				Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y);
			focalDirtyRight = false;
		}
		gapDirty = false;

		// 任意ログ
		if (gamepadIndex >= 0 && currentTime - lastLog > 1.0) { logJoystickButtonsAxes(gamepadIndex); lastLog = currentTime; }

		// 各ウィンドウのWASD移動（1px/Shift=10px）
		updateWindowPositionDiscrete(winLeft, offsetXLeft, offsetYLeft, mx, my, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, moveLeftState);
		updateWindowPositionDiscrete(winRight, offsetXRight, offsetYRight, mx, my, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, moveRightState);

		// ゲームパッド再接続検知
		if (gamepadIndex >= 0 && glfwJoystickPresent(gamepadIndex) != GLFW_TRUE) {
			std::cout << "ゲームパッド切断検出。再接続待機...\n";
			gamepadIndex = -1;
		}
		if (gamepadIndex < 0) {
			// 再接続待機（非ブロッキング的: 毎フレーム少し待つ）
			static double lastRetry = 0.0;
			if (currentTime - lastRetry > 0.5) {
				lastRetry = currentTime;
				for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
					if (glfwJoystickPresent(jid) == GLFW_TRUE) {
						gamepadIndex = jid;
						const char* joyName = glfwGetJoystickName(gamepadIndex);
						std::cout << "再接続: id=" << gamepadIndex
							<< " name='" << (joyName ? joyName : "unknown") << "' "
							<< (glfwJoystickIsGamepad(gamepadIndex) ? "[mapped]" : "[raw]") << "\n";
						break;
					}
				}
			}
		}

		// ==== ここから体積化 ====
		// フレーム取得
		auto frames = pipe.wait_for_frames();
		frames = align_to_color.process(frames);
		rs2::video_frame color_frame = frames.get_color_frame();
		rs2::depth_frame depth_frame = frames.get_depth_frame();

		// 点群生成
		points = pc.calculate(depth_frame);
		pc.map_to(color_frame);
		auto verts = points.get_vertices();

		// カラー画像ラップ
		cv::Mat color_image(cv::Size(RGBD_IMG_WIDTH, RGBD_IMG_HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);

		// ==== 変更: 体積化ステップ 左右分離（旧単一バッファ処理ブロック全体差し替え） ====
		// 1) 左右ゼロクリア
		{
			vector<thread> threads;
			int rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(zerosBox, start, end, redLeft, greenLeft, blueLeft, numPointsLeft);
			}
			for (auto& t : threads) t.join();
			threads.clear();
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(zerosBox, start, end, redRight, greenRight, blueRight, numPointsRight);
			}
			for (auto& t : threads) t.join();
		}
		// 共通取得済み: frames, points, color_image
		const rs2::vertex* vtx = points.get_vertices();
		const int W = RGBD_IMG_WIDTH;
		const int H = RGBD_IMG_HEIGHT;
		const bool cont = color_image.isContinuous();
		const cv::Vec3b* pixPtr = cont ? color_image.ptr<cv::Vec3b>(0) : nullptr;

		// 2) 左ボリュームビニング
		{
			const float fOverZPlanePitchL = focalLengthLeft / Z_PLANE_IMG_PITCH;
			for (int i = 0; i < W * H; ++i) {
				const rs2::vertex& v = vtx[i];
				float tmp_pcd_x = v.x + diffXLeft;
				float tmp_pcd_y = -v.y + diffHeightLeft;
				float tmp_pcd_z = v.z + DISTANCE_CAMERA_DISPLAY;
				if (tmp_pcd_z <= 0.0f) continue;

				unsigned char b, g, r;
				if (cont) { const cv::Vec3b& c = pixPtr[i]; b = c[0]; g = c[1]; r = c[2]; }
				else {
					int row = i / W; int col = i - row * W;
					const cv::Vec3b& c = color_image.at<cv::Vec3b>(row, col);
					b = c[0]; g = c[1]; r = c[2];
				}
				float invz = 1.0f / tmp_pcd_z;
				int nx = (int)lroundf(fOverZPlanePitchL * (tmp_pcd_x * invz)) + HALF_Z_PLANE_IMG_PX_X;
				int ny = (int)lroundf(fOverZPlanePitchL * (tmp_pcd_y * invz)) + HALF_Z_PLANE_IMG_PX_Y;
				int nz = (int)std::floor(COEF_TRANS * invz + 0.5f);
				if (0 <= nz && nz < NUM_Z_PLANE) {
					for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; ++m) {
						for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; ++n) {
							int x = nx + n, y = ny + m;
							if ((unsigned)x < (unsigned)Z_PLANE_IMG_PX_X && (unsigned)y < (unsigned)Z_PLANE_IMG_PX_Y) {
								int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
								blueLeft[idx] += b; greenLeft[idx] += g; redLeft[idx] += r; numPointsLeft[idx]++;
							}
						}
					}
				}
			}
		}
		// 3) 右ボリュームビニング
		{
			const float fOverZPlanePitchR = focalLengthRight / Z_PLANE_IMG_PITCH;
			for (int i = 0; i < W * H; ++i) {
				const rs2::vertex& v = vtx[i];
				float tmp_pcd_x = v.x + diffXRight;
				float tmp_pcd_y = -v.y + diffHeightRight;
				float tmp_pcd_z = v.z + DISTANCE_CAMERA_DISPLAY;
				if (tmp_pcd_z <= 0.0f) continue;

				unsigned char b, g, r;
				if (cont) { const cv::Vec3b& c = pixPtr[i]; b = c[0]; g = c[1]; r = c[2]; }
				else {
					int row = i / W; int col = i - row * W;
					const cv::Vec3b& c = color_image.at<cv::Vec3b>(row, col);
					b = c[0]; g = c[1]; r = c[2];
				}
				float invz = 1.0f / tmp_pcd_z;
				int nx = (int)lroundf(fOverZPlanePitchR * (tmp_pcd_x * invz)) + HALF_Z_PLANE_IMG_PX_X;
				int ny = (int)lroundf(fOverZPlanePitchR * (tmp_pcd_y * invz)) + HALF_Z_PLANE_IMG_PX_Y;
				int nz = (int)std::floor(COEF_TRANS * invz + 0.5f);
				if (0 <= nz && nz < NUM_Z_PLANE) {
					for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; ++m) {
						for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; ++n) {
							int x = nx + n, y = ny + m;
							if ((unsigned)x < (unsigned)Z_PLANE_IMG_PX_X && (unsigned)y < (unsigned)Z_PLANE_IMG_PX_Y) {
								int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
								blueRight[idx] += b; greenRight[idx] += g; redRight[idx] += r; numPointsRight[idx]++;
							}
						}
					}
				}
			}
		}
		// 4) 平均化（左右）
		{
			vector<thread> threads;
			int rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(averageSlices, start, end, redLeft, greenLeft, blueLeft, numPointsLeft);
			}
			for (auto& t : threads) t.join();
			threads.clear();
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(averageSlices, start, end, redRight, greenRight, blueRight, numPointsRight);
			}
			for (auto& t : threads) t.join();
		}
		// 5) RGBA パック（左右）
		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlicesLeft);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		{
			vector<thread> threads;
			int rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(packSlicesRGBA, start, end, redLeft, greenLeft, blueLeft, numPointsLeft, rgba3DLeft.data());
			}
			for (auto& t : threads) t.join();
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE,
			GL_RGBA, GL_UNSIGNED_BYTE, rgba3DLeft.data());
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		glBindTexture(GL_TEXTURE_2D_ARRAY, texSlicesRight);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		{
			vector<thread> threads;
			int rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
			for (int i = 0; i < NUM_THREADS; ++i) {
				int start = i * rowsPerThread;
				int end = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
				threads.emplace_back(packSlicesRGBA, start, end, redRight, greenRight, blueRight, numPointsRight, rgba3DRight.data());
			}
			for (auto& t : threads) t.join();
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, NUM_Z_PLANE,
			GL_RGBA, GL_UNSIGNED_BYTE, rgba3DRight.data());
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		// ==== 変更: 左右描画呼び出しの baseOffset / texSlices ====
		// 左
		drawTiles(winLeft, compose, baseOffsetLeft.data(),
			-HALF_NUM_LENS_X, LEFT_COLS,
			-HALF_NUM_LENS_Y, NUM_LENS_Y,
			NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y,
			HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
			NZ, locInvZLeft, invZ.data(), uvScaleX, uvScaleY,
			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, invZo, texSlicesLeft, quadVAO);
		// 右
		drawTiles(winRight, compose, baseOffsetRight.data(),
			0, RIGHT_COLS,
			-HALF_NUM_LENS_Y, NUM_LENS_Y,
			NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y,
			HALF_NUM_LENS_X, HALF_NUM_LENS_Y,
			NZ, locInvZRight, invZ.data(), uvScaleX, uvScaleY,
			Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, invZo, texSlicesRight, quadVAORight);

		// イベント
		glfwPollEvents();

		// ==== 計測終了 ====
		// GPU計測終了（右描画でコンテキストが右になっている可能性があるため左へ戻す）
		glfwMakeContextCurrent(winLeft);
		glEndQuery(GL_TIME_ELAPSED);
		GLuint64 gpuElapsedNs = 0;
		glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &gpuElapsedNs);
		glDeleteQueries(1, &timeQuery);
		double gpuMs = static_cast<double>(gpuElapsedNs) / 1.0e6;

		auto cpuEnd = std::chrono::high_resolution_clock::now();
		double cpuMs = std::chrono::duration<double, std::milli>(cpuEnd - cpuStart).count();

		// 初回フレームはウォームアップとみなし除外（必要なければ条件を削除）
		if (frameCount > 0) {
			sum_cpu_time_ms += cpuMs;
			sum_gpu_time_ms += gpuMs;
		}
		frameCount++;

	}

	// （追加）メインループ終了後（後処理の直前あたり）に平均出力
	std::cout << "フレーム数:" << frameCount << std::endl;
	if (frameCount > 1) {
		std::cout << "平均CPU実行時間: " << (sum_cpu_time_ms / (frameCount - 1)) << " ms" << std::endl;
		std::cout << "平均GPU実行時間: " << (sum_gpu_time_ms / (frameCount - 1)) << " ms" << std::endl;
	}

	// ==== 変更: 終了時 CSV 保存引数（左右独立値） ====
	saveSettingsCSV("settings.csv",
		offsetXLeft, offsetYLeft, offsetXRight, offsetYRight,
		diffXLeft, diffHeightLeft, focalLengthLeft,
		diffXRight, diffHeightRight, focalLengthRight,
		centerDistance);
	std::cout << "[Settings] 現在値を settings.csv に保存しました\n";

	//// 表示画像の保存
	////------------------------------

	//vector<unsigned char> buf(NUM_DISPLAY_IMG_PX_X * NUM_DISPLAY_IMG_PX_Y * 3);
	//glReadBuffer(GL_FRONT);
	//glReadPixels(0, 0, NUM_DISPLAY_IMG_PX_X, NUM_DISPLAY_IMG_PX_Y, GL_RGB, GL_UNSIGNED_BYTE, buf.data());

	//cv::Mat img(NUM_DISPLAY_IMG_PX_Y, NUM_DISPLAY_IMG_PX_X, CV_8UC3);
	//for (int y = 0; y < NUM_DISPLAY_IMG_PX_Y; ++y) {
	//	unsigned char* dst = img.ptr<unsigned char>(NUM_DISPLAY_IMG_PX_Y - 1 - y);
	//	const unsigned char* src = buf.data() + y * NUM_DISPLAY_IMG_PX_X * 3;
	//	for (int x = 0; x < NUM_DISPLAY_IMG_PX_X; ++x) {
	//		dst[x * 3 + 0] = src[x * 3 + 2]; // B
	//		dst[x * 3 + 1] = src[x * 3 + 1]; // G
	//		dst[x * 3 + 2] = src[x * 3 + 0]; // R
	//	}
	//}

	//std::ostringstream stream;
	//stream << "D:/ForStudy/reconstruction/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1-milkdrop_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << ".png";
	//std::string outPath = stream.str();

	//cv::imwrite(outPath, img);

	////------------------------------

	//// 3次元配列のテクスチャ画像の保存
	////------------------------------

	//std::ostringstream stream;
	//cv::Mat sliceImage = cv::Mat::zeros(Z_PLANE_IMG_PX_Y, Z_PLANE_IMG_PX_X, CV_8UC3);
	//int count_color;
	//for (int n = 0; n < NUM_Z_PLANE; n++)
	//{
	//	stream.str("");
	//	stream << "D:/ForStudy/slice-images/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1/PCSJ2025-OpenGL-" << VIEW_MODE << "-v1-pepper_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << "_nz" << n << ".png";

	//	count_color = 0;
	//	sliceImage = cv::Scalar(0);

	//	for (int y = 0; y < Z_PLANE_IMG_PX_Y; y++)
	//	{
	//		int idx_y = Z_PLANE_IMG_PX_Y - y - 1;
	//		for (int x = 0; x < Z_PLANE_IMG_PX_X; x++)
	//		{
	//			int idx = (n * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
	//			sliceImage.at<cv::Vec3b>(idx_y, x)[0] = blue[idx];
	//			sliceImage.at<cv::Vec3b>(idx_y, x)[1] = green[idx];
	//			sliceImage.at<cv::Vec3b>(idx_y, x)[2] = red[idx];

	//			if (blue[idx] + green[idx] + red[idx] > 0)
	//			{
	//				count_color++;
	//			}
	//		}
	//	}

	//	if (count_color > 0)
	//	{
	//		std::string outPath = stream.str();
	//		cv::imwrite(outPath, sliceImage);
	//	}

	//}

	////------------------------------

	// 後処理
	//------------------------------

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAORight);
	glDeleteBuffers(1, &quadVBORight);
	// ==== 変更: 旧単一バッファ delete を左右対応へ（後処理部） ====
	glDeleteTextures(1, &texSlicesLeft);
	glDeleteTextures(1, &texSlicesRight);
	delete[] camPosLeft;
	delete[] camPosRight;
	delete[] redLeft; delete[] greenLeft; delete[] blueLeft; delete[] numPointsLeft;
	delete[] redRight; delete[] greenRight; delete[] blueRight; delete[] numPointsRight;
	glfwTerminate();

	//------------------------------

	return 0;
}

//--------------------------------- ユーティリティ ---------------------------------

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void zerosBox(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, unsigned int* numPoints) {
	const size_t planePixels = size_t(Z_PLANE_IMG_PX_X) * Z_PLANE_IMG_PX_Y;
	const size_t offset = size_t(start) * planePixels;
	const size_t count = size_t(end - start) * planePixels;
	std::memset(red + offset, 0, count * sizeof(unsigned int));
	std::memset(green + offset, 0, count * sizeof(unsigned int));
	std::memset(blue + offset, 0, count * sizeof(unsigned int));
	std::memset(numPoints + offset, 0, count * sizeof(unsigned int));
}

void averageSlices(int start, int end, unsigned int* red, unsigned int* green, unsigned int* blue, const unsigned int* numPoints) {
	for (int i = start; i < end; ++i) {
		const int base = i * Z_PLANE_IMG_PX_Y * Z_PLANE_IMG_PX_X;
		for (int y = 0; y < Z_PLANE_IMG_PX_Y; ++y) {
			for (int x = 0; x < Z_PLANE_IMG_PX_X; ++x) {
				const int idx = base + y * Z_PLANE_IMG_PX_X + x;
				const unsigned int cnt = numPoints[idx];
				if (cnt) {
					red[idx] = (red[idx] + (cnt >> 1)) / cnt;
					green[idx] = (green[idx] + (cnt >> 1)) / cnt;
					blue[idx] = (blue[idx] + (cnt >> 1)) / cnt;
				}
			}
		}
	}
}

void packSlicesRGBA(int start, int end, const unsigned int* red, const unsigned int* green, const unsigned int* blue, const unsigned int* numPoints, unsigned char* rgba3D) {
	const int layerPitch = Z_PLANE_IMG_PX_X * Z_PLANE_IMG_PX_Y * 4;
	for (int nz = start; nz < end; ++nz) {
		unsigned char* layerOut = rgba3D + nz * layerPitch;
		for (int y = 0; y < Z_PLANE_IMG_PX_Y; ++y) {
			for (int x = 0; x < Z_PLANE_IMG_PX_X; ++x) {
				const int idx = (nz * Z_PLANE_IMG_PX_Y + y) * Z_PLANE_IMG_PX_X + x;
				const int out = (y * Z_PLANE_IMG_PX_X + x) * 4;
				layerOut[out + 0] = static_cast<unsigned char>((std::min)(red[idx], 255u));
				layerOut[out + 1] = static_cast<unsigned char>((std::min)(green[idx], 255u));
				layerOut[out + 2] = static_cast<unsigned char>((std::min)(blue[idx], 255u));
				layerOut[out + 3] = (numPoints[idx] > 0u) ? 255u : 0u;
			}
		}
	}
}

// 離散1回押し移動（1px / Shift同時=10px）
static void updateWindowPositionDiscrete(
	GLFWwindow* window, int& offsetX, int& offsetY, int monX, int monY,
	int upKey, int downKey, int leftKey, int rightKey, WindowMoveState& state)
{
	int curUp = glfwGetKey(window, upKey);
	int curDown = glfwGetKey(window, downKey);
	int curLeft = glfwGetKey(window, leftKey);
	int curRight = glfwGetKey(window, rightKey);
	bool shiftHeld =
		(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
		(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

	int step = shiftHeld ? 10 : 1;
	auto pressedOnce = [](int cur, int prev) { return prev == GLFW_RELEASE && cur == GLFW_PRESS; };

	bool moved = false;
	if (pressedOnce(curUp, state.prevUp)) offsetY -= step;
	if (pressedOnce(curDown, state.prevDown)) offsetY += step;
	if (pressedOnce(curLeft, state.prevLeft)) offsetX -= step;
	if (pressedOnce(curRight, state.prevRight)) offsetX += step;

	state.prevUp = curUp;
	state.prevDown = curDown;
	state.prevLeft = curLeft;
	state.prevRight = curRight;

	glfwSetWindowPos(window, monX + offsetX, monY + offsetY);
	if (moved) {
		printMoveAmounts(offsetX, offsetY);
	}
}

static void focusWindowPortable(GLFWwindow* w)
{
#if defined(_WIN32)
	HWND hwnd = glfwGetWin32Window(w);
	if (hwnd) {
		ShowWindow(hwnd, SW_SHOW);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetForegroundWindow(hwnd);
		SetFocus(hwnd);
		BringWindowToTop(hwnd);
	}
#else
	glfwShowWindow(w);
#endif
}

static void toggleFocusWithKey(GLFWwindow* winLeft, GLFWwindow* winRight)
{
	static int prevF = GLFW_RELEASE;

	GLFWwindow* focused = nullptr;
	if (glfwGetWindowAttrib(winLeft, GLFW_FOCUSED) == GLFW_TRUE) focused = winLeft;
	else if (glfwGetWindowAttrib(winRight, GLFW_FOCUSED) == GLFW_TRUE) focused = winRight;
	else return;

	const int curF = glfwGetKey(focused, GLFW_KEY_F);
	if (prevF == GLFW_RELEASE && curF == GLFW_PRESS) {
		GLFWwindow* target = (focused == winLeft) ? winRight : winLeft;
		focusWindowPortable(target);
	}
	prevF = curF;
}

static void logAllJoysticksOnce() {
	std::cout << "[Joystick Scan] Begin\n";
	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
		if (glfwJoystickPresent(jid) == GLFW_TRUE) {
			const char* name = glfwGetJoystickName(jid);
			bool mapped = glfwJoystickIsGamepad(jid) == GLFW_TRUE;
			int axisCount = 0; glfwGetJoystickAxes(jid, &axisCount);
			int btnCount = 0; glfwGetJoystickButtons(jid, &btnCount);
			int hatCount = 0; glfwGetJoystickHats(jid, &hatCount);
			std::cout << "  jid=" << jid << " name='" << (name ? name : "null") << "' mapped=" << (mapped ? "YES" : "NO")
				<< " axes=" << axisCount << " buttons=" << btnCount << " hats=" << hatCount << "\n";
		}
	}
	std::cout << "[Joystick Scan] End\n";
}

static int selectPrimaryGamepad() {
	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
		if (glfwJoystickPresent(jid) == GLFW_TRUE) {
			if (glfwJoystickIsGamepad(jid) == GLFW_TRUE) { std::cout << "[GamepadSelect] Use mapped " << jid << "\n"; return jid; }
			if (jid == GLFW_JOYSTICK_1) return jid; // fallback
		}
	}
	std::cout << "[GamepadSelect] No joystick present\n";
	return -1;
}

static void logJoystickButtonsAxes(int jid) {
	if (jid < GLFW_JOYSTICK_1 || jid > GLFW_JOYSTICK_LAST) return;
	if (glfwJoystickPresent(jid) != GLFW_TRUE) return;

	int axisCount = 0, btnCount = 0, hatCount = 0;
	const float* axes = glfwGetJoystickAxes(jid, &axisCount);
	const unsigned char* btns = glfwGetJoystickButtons(jid, &btnCount);
	const unsigned char* hats = glfwGetJoystickHats(jid, &hatCount);

	//std::cout << "[JoystickState] jid=" << jid << " axes(" << axisCount << "):";
	//for (int i = 0; i < axisCount; ++i) std::cout << axes[i] << ",";
	//std::cout << " buttons(" << btnCount << "):";
	//for (int i = 0; i < btnCount; ++i) std::cout << (int)btns[i];
	//std::cout << " hats(" << hatCount << "):";
	//for (int i = 0; i < hatCount; ++i) std::cout << (int)hats[i] << " ";
	//std::cout << "\n";
}

// ==== 変更: processGamepadDiscrete 実装本体（左右フォーカスに応じて調整） ====
// 既存実装差し替え（要点のみ抜粋）
static void processGamepadDiscrete(GLFWwindow* winLeft, GLFWwindow* winRight,
	int& offsetXLeft, int& offsetYLeft, int& offsetXRight, int& offsetYRight,
	int monX, int monY, GamepadEdgeState& gp, int gamepadIndex,
	float& centerDistance, bool& gapDirty, OperationMode& controlMode,
	float& focalLengthLeft, bool& focalDirtyLeft,
	float& focalLengthRight, bool& focalDirtyRight,
	float& diffHeightLeft, bool& heightDirtyLeft,
	float& diffHeightRight, bool& heightDirtyRight,
	float& diffXLeft, bool& xDirtyLeft,
	float& diffXRight, bool& xDirtyRight)
{
	if (gamepadIndex < GLFW_JOYSTICK_1 || gamepadIndex > GLFW_JOYSTICK_LAST) return;
	if (glfwJoystickPresent(gamepadIndex) != GLFW_TRUE) return;

	GLFWwindow* focused = nullptr;
	bool focusedIsLeft = false;
	if (glfwGetWindowAttrib(winLeft, GLFW_FOCUSED) == GLFW_TRUE) { focused = winLeft; focusedIsLeft = true; }
	else if (glfwGetWindowAttrib(winRight, GLFW_FOCUSED) == GLFW_TRUE) { focused = winRight; focusedIsLeft = false; }

	unsigned char curUp = 0, curDown = 0, curLeft = 0, curRight = 0, curLB = 0, curRB = 0, curX = 0, curBack = 0, curY = 0;
	int btnCount = 0;
	if (glfwJoystickIsGamepad(gamepadIndex) == GLFW_TRUE) {
		GLFWgamepadstate st{};
		if (glfwGetGamepadState(gamepadIndex, &st) == GLFW_TRUE) {
			curUp = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
			curDown = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
			curLeft = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
			curRight = st.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
			curLB = st.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
			curRB = st.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
			curX = st.buttons[GLFW_GAMEPAD_BUTTON_X];
			curY = st.buttons[GLFW_GAMEPAD_BUTTON_Y];
			curBack = st.buttons[GLFW_GAMEPAD_BUTTON_BACK];
		}
	}
	else {
		const unsigned char* hats = glfwGetJoystickHats(gamepadIndex, &btnCount);
		if (btnCount > 0) {
			unsigned char h = hats[0];
			if (h & GLFW_HAT_UP) curUp = 1;
			if (h & GLFW_HAT_DOWN) curDown = 1;
			if (h & GLFW_HAT_LEFT) curLeft = 1;
			if (h & GLFW_HAT_RIGHT) curRight = 1;
		}
		const unsigned char* btns = glfwGetJoystickButtons(gamepadIndex, &btnCount);
		if (btnCount >= 7) { curLB = btns[4]; curRB = btns[5]; curX = btns[2]; curBack = btns[6]; }
		if (btnCount >= 4) { curY = btns[3]; }
	}

	auto pressedOnce = [&](unsigned char cur, unsigned char prev) {return prev == 0 && cur == 1; };

	// Xでフォーカス切替
	if (pressedOnce(curX, gp.x)) {
		GLFWwindow* target = (focused == winLeft) ? winRight : winLeft;
		if (!focused) target = winLeft;
		focusWindowPortable(target);
		focused = target;
		focusedIsLeft = (target == winLeft);
		if (controlMode == OperationMode::WindowMove) {
			std::cout << "[Focus] ウィンドウ移動モード: 現在フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ\n";
		}
		else if (controlMode == OperationMode::PointCloudShiftAdjust) {
			std::cout << "[Focus] 点群水平位置調整モード: 現在フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ\n";
		}
		else if (controlMode == OperationMode::FocalLengthAdjust) {
			std::cout << "[Focus] 焦点距離調整モード: 現在フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ\n";
		}

	}
	// Back
	if (pressedOnce(curBack, gp.back) && focused) glfwSetWindowShouldClose(focused, true);

	// Yでモード循環
	if (pressedOnce(curY, gp.y)) {
		switch (controlMode) {
		case OperationMode::WindowMove:
			controlMode = OperationMode::CenterDistanceAdjust;
			std::cout << "[Mode] centerDistance調整モードへ\n"; break;
		case OperationMode::CenterDistanceAdjust:
			controlMode = OperationMode::PointCloudShiftAdjust;
			std::cout << "[Mode] 点群水平位置調整モードへ (L:dx=" << diffXLeft << " dy=" << diffHeightLeft << ", R:dx=" << diffXRight << " dy=" << diffHeightRight
				<< "); フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ)\n"; break;
		case OperationMode::PointCloudShiftAdjust:
			controlMode = OperationMode::FocalLengthAdjust;
			std::cout << "[Mode] 焦点距離調整モードへ (fL=" << focalLengthLeft << " fR=" << focalLengthRight
				<< "); フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ)\n"; break;
		case OperationMode::FocalLengthAdjust:
			controlMode = OperationMode::WindowMove;
			std::cout << "[Mode] ウィンドウ移動モードへ (L:dx=" << offsetXLeft << " dy=" << offsetYLeft << ", R:dx=" << offsetXRight << " dy=" << offsetYRight
				<< "); フォーカス=" << (focusedIsLeft ? "左" : "右") << "ウィンドウ)\n"; break;
		}
	}

	// CenterDistanceAdjust
	if (controlMode == OperationMode::CenterDistanceAdjust) {
		float step = (curLB || curRB) ? CENTER_DISTANCE_STEP_COARSE : CENTER_DISTANCE_STEP_FINE;
		bool changed = false;
		if (pressedOnce(curUp, gp.up)) { centerDistance += step; changed = true; }
		if (pressedOnce(curDown, gp.down)) { centerDistance = std::max(0.0f, centerDistance - step); changed = true; }
		if (changed) { gapDirty = true; std::cout << "[centerDistance]=" << centerDistance << " (step=" << step << ")\n"; }
	}
	// PointCloudShiftAdjust (左右フォーカス毎に独立)
	else if (controlMode == OperationMode::PointCloudShiftAdjust) {
		float stepH = (curLB || curRB) ? HEIGHT_STEP_COARSE : HEIGHT_STEP_FINE;
		float stepX = (curLB || curRB) ? X_STEP_COARSE : X_STEP_FINE;
		bool ch = false;
		if (focusedIsLeft) {
			if (pressedOnce(curUp, gp.up)) { diffHeightLeft = std::min(diffHeightLeft + stepH, HEIGHT_MAX); ch = true; }
			if (pressedOnce(curDown, gp.down)) { diffHeightLeft = std::max(diffHeightLeft - stepH, HEIGHT_MIN); ch = true; }
			if (pressedOnce(curRight, gp.right)) { diffXLeft = std::min(diffXLeft + stepX, X_MAX); ch = true; }
			if (pressedOnce(curLeft, gp.left)) { diffXLeft = std::max(diffXLeft - stepX, X_MIN); ch = true; }
			if (ch) {
				heightDirtyLeft = true; xDirtyLeft = true;
				std::cout << "[Left shift] diffX=" << diffXLeft << " diffHeight=" << diffHeightLeft << " (stepX=" << stepX << ", stepH=" << stepH << ")\n";
			}
		}
		else if (focused) {
			if (pressedOnce(curUp, gp.up)) { diffHeightRight = std::min(diffHeightRight + stepH, HEIGHT_MAX); ch = true; }
			if (pressedOnce(curDown, gp.down)) { diffHeightRight = std::max(diffHeightRight - stepH, HEIGHT_MIN); ch = true; }
			if (pressedOnce(curRight, gp.right)) { diffXRight = std::min(diffXRight + stepX, X_MAX); ch = true; }
			if (pressedOnce(curLeft, gp.left)) { diffXRight = std::max(diffXRight - stepX, X_MIN); ch = true; }
			if (ch) {
				heightDirtyRight = true; xDirtyRight = true;
				std::cout << "[Right shift] diffX=" << diffXRight << " diffHeight=" << diffHeightRight << " (stepX=" << stepX << ", stepH=" << stepH << ")\n";
			}
		}
	}
	// FocalLengthAdjust (左右独立)
	else if (controlMode == OperationMode::FocalLengthAdjust) {
		float stepF = (curLB || curRB) ? FOCAL_STEP_COARSE : FOCAL_STEP_FINE;
		bool ch = false;
		if (focusedIsLeft) {
			if (pressedOnce(curUp, gp.up)) { focalLengthLeft = std::min(focalLengthLeft + stepF, FOCAL_MAX); ch = true; }
			if (pressedOnce(curDown, gp.down)) { focalLengthLeft = std::max(focalLengthLeft - stepF, FOCAL_MIN); ch = true; }
			if (ch) { focalDirtyLeft = true; std::cout << "[focalLeft]=" << focalLengthLeft << " (step=" << stepF << ")\n"; }
		}
		else if (focused) {
			if (pressedOnce(curUp, gp.up)) { focalLengthRight = std::min(focalLengthRight + stepF, FOCAL_MAX); ch = true; }
			if (pressedOnce(curDown, gp.down)) { focalLengthRight = std::max(focalLengthRight - stepF, FOCAL_MIN); ch = true; }
			if (ch) { focalDirtyRight = true; std::cout << "[focalRight]=" << focalLengthRight << " (step=" << stepF << ")\n"; }
		}
	}
	// WindowMove
	else if (controlMode == OperationMode::WindowMove) {
		int step = (curLB || curRB) ? 10 : 1;
		if (focused) {
			bool mv = false;
			if (pressedOnce(curUp, gp.up)) { if (focusedIsLeft) offsetYLeft -= step; else offsetYRight -= step; mv = true; }
			if (pressedOnce(curDown, gp.down)) { if (focusedIsLeft) offsetYLeft += step; else offsetYRight += step; mv = true; }
			if (pressedOnce(curLeft, gp.left)) { if (focusedIsLeft) offsetXLeft -= step; else offsetXRight -= step; mv = true; }
			if (pressedOnce(curRight, gp.right)) { if (focusedIsLeft) offsetXLeft += step; else offsetXRight += step; mv = true; }
			if (mv) {
				if (focusedIsLeft) glfwSetWindowPos(winLeft, monX + offsetXLeft, monY + offsetYLeft);
				else glfwSetWindowPos(winRight, monX + offsetXRight, monY + offsetYRight);
				printMoveAmounts(focusedIsLeft ? offsetXLeft : offsetXRight,
					focusedIsLeft ? offsetYLeft : offsetYRight);
			}
		}
	}

	gp.up = curUp; gp.down = curDown; gp.left = curLeft; gp.right = curRight;
	gp.x = curX; gp.back = curBack; gp.y = curY;
	gp.a = 0;
}

static void drawTiles(
	GLFWwindow* win, Shader& compose,
	const glm::vec2* baseOffset, int lensColsStart, int lensColsCount,
	int lensRowsStart, int lensRowsCount,
	int tileW, int tileH,
	int halfCols, int halfRows,
	int nz, GLint locInvZ, const float* invZ, float uvScaleX, float uvScaleY,
	int texW, int texH, float invZo, GLuint texSlices, GLuint quadVAO)
{
	glfwMakeContextCurrent(win);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	compose.use();
	compose.setInt("uSlices", 0);
	compose.setInt("uNZ", nz);
	compose.setVec2("uUvScale", glm::vec2(uvScaleX, uvScaleY));
	compose.setVec2("uTexelSize", glm::vec2(1.f / texW, 1.f / texH));
	glUniform1fv(locInvZ, nz, invZ);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texSlices);

	glEnable(GL_SCISSOR_TEST);
	glBindVertexArray(quadVAO);

	for (int r = lensRowsStart; r < lensRowsStart + lensRowsCount; ++r) {
		int rowIdx = r + halfRows;
		int localRow = r - lensRowsStart;
		for (int c = lensColsStart; c < lensColsStart + lensColsCount; ++c) {
			int lensColIdx = c + halfCols;
			int localCol = c - lensColsStart;
			int idx = rowIdx * (halfCols * 2) + lensColIdx;

			int vx = localCol * tileW;
			int vy = localRow * tileH;
			glViewport(vx, vy, tileW, tileH);
			glScissor(vx, vy, tileW, tileH);

			compose.setVec2("uBaseOffset", baseOffset[idx]);
			glm::vec2 frustumUVShift = baseOffset[idx] * invZo;
			compose.setVec2("uFrustumUVShift", frustumUVShift);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	glBindVertexArray(0);
	glDisable(GL_SCISSOR_TEST);
	glUseProgram(0);
	glfwSwapBuffers(win);
}

// （追加）移動量表示ヘルパ（updateWindowPositionDiscrete / processGamepadDiscrete から使用）
// ウィンドウ座標系は下方向が+なので表示は上下で符号反転し「上を正」にする
static inline void printMoveAmounts(int offsetX, int offsetY) {
	int dispVert = offsetY;      // 下方向を正
	int dispHorz = offsetX;       // 右方向を正
	std::cout << "[移動量]上下:" << (dispVert >= 0 ? "+" : "") << dispVert << " px, 左右:"
		<< (dispHorz >= 0 ? "+" : "") << dispHorz << " px\n";
}

// ギャップ適用版のカメラ配置と baseOffset 再計算
static void recomputeCameraLayout(float gap,
	glm::vec3* camPos,
	std::vector<glm::vec2>& baseOffset,
	int numLensX, int numLensY,
	int halfX, int halfY,
	float halfPitchX, float halfPitchY,
	float focalLen, float imgPitch, int texW, int texH)
{
	const float scaleX = focalLen / imgPitch / (float)texW;
	const float scaleY = focalLen / imgPitch / (float)texH;

	for (int r = -halfY; r < halfY; ++r) {
		int row = r + halfY;
		float camPosY = (2.0f * (float)r + 1.0f) * halfPitchY;
		for (int c = -halfX; c < halfX; ++c) {
			int col = c + halfX;
			float camPosX = (2.0f * (float)c + 1.0f) * halfPitchX;
			if (gap != 0.0f) {
				// 左右ウィンドウ境界（c = -1 と 0 の間）に gap を等分シフト（重心保持）
				camPosX += (c >= 0 ? +0.5f : -0.5f) * gap;
			}
			int idx = row * numLensX + col;
			camPos[idx] = glm::vec3(camPosX, camPosY, 0.0f);
			baseOffset[idx] = glm::vec2(camPosX * scaleX, camPosY * scaleY);
		}
	}
}

// ==== 追加: 新CSV関数実装（旧 load/saveSettingsCSV 実装差し替え） ====
static bool loadSettingsCSV(const std::string& path,
	int& offsetXLeft, int& offsetYLeft, int& offsetXRight, int& offsetYRight,
	float& diffXLeft, float& diffHeightLeft, float& focalLengthLeft,
	float& diffXRight, float& diffHeightRight, float& focalLengthRight,
	float& centerDistance)
{
	std::ifstream ifs(path);
	if (!ifs.is_open()) return false;
	std::string header;
	if (!std::getline(ifs, header)) return false;
	std::string line;
	if (!std::getline(ifs, line)) return false;
	std::stringstream ss(line);
	std::string cell;
	std::vector<std::string> cells;
	while (std::getline(ss, cell, ',')) cells.push_back(cell);

	// 新フォーマット(11項目) or 旧フォーマット(8項目)判定
	if (cells.size() == 11) {
		try {
			offsetXLeft = std::stoi(cells[0]);
			offsetYLeft = std::stoi(cells[1]);
			offsetXRight = std::stoi(cells[2]);
			offsetYRight = std::stoi(cells[3]);
			diffXLeft = std::stof(cells[4]);
			diffHeightLeft = std::stof(cells[5]);
			focalLengthLeft = std::stof(cells[6]);
			diffXRight = std::stof(cells[7]);
			diffHeightRight = std::stof(cells[8]);
			focalLengthRight = std::stof(cells[9]);
			centerDistance = std::stof(cells[10]);
		}
		catch (...) { return false; }
	}
	else if (cells.size() == 8) {
		// 旧フォーマットを左右同一として適用
		try {
			offsetXLeft = std::stoi(cells[0]);
			offsetYLeft = std::stoi(cells[1]);
			offsetXRight = std::stoi(cells[2]);
			offsetYRight = std::stoi(cells[3]);
			diffXLeft = std::stof(cells[4]);
			diffHeightLeft = std::stof(cells[5]);
			focalLengthLeft = std::stof(cells[6]);
			centerDistance = std::stof(cells[7]);
			// 右へコピー
			diffXRight = diffXLeft;
			diffHeightRight = diffHeightLeft;
			focalLengthRight = focalLengthLeft;
		}
		catch (...) { return false; }
	}
	else {
		return false;
	}

	auto clampF = [](float v, float mn, float mx) { return std::max(mn, std::min(mx, v)); };
	focalLengthLeft = clampF(focalLengthLeft, FOCAL_MIN, FOCAL_MAX);
	focalLengthRight = clampF(focalLengthRight, FOCAL_MIN, FOCAL_MAX);
	centerDistance = std::max(0.0f, centerDistance);
	diffHeightLeft = clampF(diffHeightLeft, HEIGHT_MIN, HEIGHT_MAX);
	diffHeightRight = clampF(diffHeightRight, HEIGHT_MIN, HEIGHT_MAX);
	diffXLeft = clampF(diffXLeft, X_MIN, X_MAX);
	diffXRight = clampF(diffXRight, X_MIN, X_MAX);
	return true;
}

static void saveSettingsCSV(const std::string& path,
	int offsetXLeft, int offsetYLeft, int offsetXRight, int offsetYRight,
	float diffXLeft, float diffHeightLeft, float focalLengthLeft,
	float diffXRight, float diffHeightRight, float focalLengthRight,
	float centerDistance)
{
	std::ofstream ofs(path, std::ios::trunc);
	if (!ofs.is_open()) {
		std::cerr << "[SaveSettings] open failed: " << path << "\n";
		return;
	}
	ofs << "offsetXLeft,offsetYLeft,offsetXRight,offsetYRight,"
		"diffXLeft,diffHeightLeft,focalLengthLeft,"
		"diffXRight,diffHeightRight,focalLengthRight,centerDistance\n";
	ofs << offsetXLeft << ',' << offsetYLeft << ',' << offsetXRight << ',' << offsetYRight << ','
		<< diffXLeft << ',' << diffHeightLeft << ',' << focalLengthLeft << ','
		<< diffXRight << ',' << diffHeightRight << ',' << focalLengthRight << ','
		<< centerDistance << '\n';
}