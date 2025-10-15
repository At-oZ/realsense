///*
//	OpenGL版vtをスクラッチで作成(learn-OpenGL-ch3ベースだが)
//	v4-1:処理速度の高速化(LODの使用)
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
//
//#include <opencv2/opencv.hpp>
//
//#include <iostream>
//#include <vector>
//#include <tuple>
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
//const unsigned int NUM_LENS_W = 128;
//const unsigned int NUM_LENS_H = 80;
//
//const int HALF_NUM_LENS_W = NUM_LENS_W * 0.5f;
//const int HALF_NUM_LENS_H = NUM_LENS_H * 0.5f;
//
//const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * (float)NUM_LENS_H - 1.0f);
//
//// レンズピッチを固定する場合(A-1)
//const float LENS_PITCH_X = 0.00451f;
//const float LENS_PITCH_Y = 0.00451f;
//const float LENS_ARRAY_W = LENS_PITCH_X * NUM_LENS_W;
//const float LENS_ARRAY_H = LENS_PITCH_Y * NUM_LENS_H;
//
////// レンズアレイ幅を固定する場合(A-2)
////const float LENS_ARRAY_W = 0.1804f;
////const float LENS_ARRAY_H = 0.1804f;
////const float LENS_PITCH_X = LENS_ARRAY_W / (float)NUM_LENS_W;
////const float LENS_PITCH_Y = LENS_ARRAY_H / (float)NUM_LENS_H;
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
//const unsigned int WIN_W = static_cast<unsigned int>(std::lround(NUM_LENS_W * ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH));
//const unsigned int WIN_H = static_cast<unsigned int>(std::lround(NUM_LENS_H * ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH));
//const unsigned int HALF_WIN_W = WIN_W / 2;
//const unsigned int HALF_WIN_H = WIN_H / 2;
//
//const float FLOAT_NUM_ELEM_IMG_PX_X = ELEM_IMG_PITCH_X / DISPLAY_PX_PITCH;
//const float FLOAT_NUM_ELEM_IMG_PX_Y = ELEM_IMG_PITCH_Y / DISPLAY_PX_PITCH;
//const unsigned int NUM_ELEM_IMG_PX_X = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_X);
//const unsigned int NUM_ELEM_IMG_PX_Y = static_cast<unsigned int>(FLOAT_NUM_ELEM_IMG_PX_Y);
//
//const float DISPLAY_AREA_SIZE_X = FLOAT_NUM_ELEM_IMG_PX_X * NUM_LENS_W * DISPLAY_PX_PITCH;
//const float DISPLAY_AREA_SIZE_Y = FLOAT_NUM_ELEM_IMG_PX_Y * NUM_LENS_H * DISPLAY_PX_PITCH;
//
//// 仮想カメラ設定
//const float TAN_HALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
//const float FOV_Y = atan(TAN_HALF_Y) * 2.0f;
////const unsigned int WIN_W = NUM_LENS_W * NUM_ELEM_IMG_PX_X;
////const unsigned int WIN_H = NUM_LENS_H * NUM_ELEM_IMG_PX_Y;
//
////------------------------------
//
//
//// 定数(被写体側)
////------------------------------
//
//const float SUBJECT_Z = 1.0f;
//const unsigned int NUM_SUBJECT_POINTS_X = 554;
//const unsigned int NUM_SUBJECT_POINTS_Y = 554;
//const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;
//const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
//const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;
//const float SUBJECT_SIZE_X = DISPLAY_AREA_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
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
//	int offsetX = 500;
//	int offsetY = 500;
//	int winW = max((int)WIN_W, mode->width - offsetX);
//	int winH = max((int)WIN_H, mode->height - offsetY);
//
//	GLFWwindow* gridWin = glfwCreateWindow(winW, winH, "Grid Cameras", nullptr, nullptr);
//	if (!gridWin) { std::fprintf(stderr, "Create window failed\n"); glfwTerminate(); return -1; }
//	glfwMakeContextCurrent(gridWin);
//	glfwSwapInterval(0);
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
//	Shader ourShader("shader-vt-v1.vert", "shader-vt-v1.frag");
//
//
//	// 点群データの生成
//	//------------------------------
//
//	// 被写体画像読み込み
//	cv::Mat image_input = cv::imread("./images/standard/grid_image.png");
//	if (image_input.empty()) {
//		std::cout << "画像を開くことができませんでした。\n";
//		return -1;
//	}
//	cv::Mat resized_image;
//	cv::resize(image_input, resized_image, cv::Size((int)NUM_SUBJECT_POINTS_Y, (int)NUM_SUBJECT_POINTS_X), 0, 0, cv::INTER_NEAREST);
//
//	// 点群
//	float* points = new float[NUM_POINTS * 6];
//	for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; ++r)
//	{
//		int row = r + HALF_NUM_SUBJECT_POINTS_Y;
//		int reverseRow = NUM_SUBJECT_POINTS_Y - 1 - row;
//
//		for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; ++c)
//		{
//			int col = c + HALF_NUM_SUBJECT_POINTS_X;
//			int idx = (row * NUM_SUBJECT_POINTS_X + col) * 6;
//
//			points[idx] = (2.0f * (float)c + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X;
//			points[idx + 1] = (2.0f * (float)r + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y;
//			points[idx + 2] = SUBJECT_Z;
//			points[idx + 3] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[2] / 255.0f);
//			points[idx + 4] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[1] / 255.0f);
//			points[idx + 5] = (float)(resized_image.at<cv::Vec3b>(reverseRow, col)[0] / 255.0f);
//
//		}
//	}
//
//	cout << "SUBJECT POINT OFFSET X:" << points[0] << endl;
//	cout << "SUBJECT POINT OFFSET Y:" << points[1] << endl;
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
//
//	// VBOとVAOの設定
//	//------------------------------
//
//	//// generate VBO and VAO
//	//unsigned int VBO, VAO;
//	//glGenVertexArrays(1, &VAO);
//	//glGenBuffers(1, &VBO);
//
//	//// bind Vertex Array Object
//	//glBindVertexArray(VAO);
//
//	//// copy our vertices array in a buffer for OpenGL to use
//	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	//glBufferData(GL_ARRAY_BUFFER, NUM_POINTS * 6 * sizeof(float), points, GL_STATIC_DRAW);
//
//	//// position attribute
//	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	//glEnableVertexAttribArray(0);
//	//// color attribute
//	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	//glEnableVertexAttribArray(1);
//
//	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//	//glBindVertexArray(0);
//
//	// stride 間引きで VAO/VBO を作るヘルパ
//	auto makePointBuffer = [&](int stride)->std::tuple<GLuint, GLuint, GLsizei> {
//		std::vector<float> buf;
//		buf.reserve((NUM_SUBJECT_POINTS_X / stride) * (NUM_SUBJECT_POINTS_Y / stride) * 6);
//		for (int r = -HALF_NUM_SUBJECT_POINTS_Y; r < HALF_NUM_SUBJECT_POINTS_Y; r += stride) {
//			int row = r + HALF_NUM_SUBJECT_POINTS_Y;
//			int rr = NUM_SUBJECT_POINTS_Y - 1 - row; // 画像の上下反転
//			for (int c = -HALF_NUM_SUBJECT_POINTS_X; c < HALF_NUM_SUBJECT_POINTS_X; c += stride) {
//				int col = c + HALF_NUM_SUBJECT_POINTS_X;
//				float px = (2.0f * c + 1.0f) * HALF_SUBJECT_POINTS_PITCH_X;
//				float py = (2.0f * r + 1.0f) * HALF_SUBJECT_POINTS_PITCH_Y;
//				buf.push_back(px);
//				buf.push_back(py);
//				buf.push_back(SUBJECT_Z);
//				cv::Vec3b bgr = resized_image.at<cv::Vec3b>(rr, col);
//				buf.push_back(bgr[2] / 255.0f);
//				buf.push_back(bgr[1] / 255.0f);
//				buf.push_back(bgr[0] / 255.0f);
//			}
//		}
//		GLuint vao = 0, vbo = 0;
//		glGenVertexArrays(1, &vao);
//		glGenBuffers(1, &vbo);
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), buf.data(), GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindVertexArray(0);
//		GLsizei count = (GLsizei)(buf.size() / 6);
//		return { vao, vbo, count };
//	};
//
//	// LOD を複数作成（必要に応じて増減）
//	GLuint vaoLOD1 = 0, vboLOD1 = 0; GLsizei countLOD1 = 0;
//	GLuint vaoLOD2 = 0, vboLOD2 = 0; GLsizei countLOD2 = 0;
//	GLuint vaoLOD3 = 0, vboLOD3 = 0; GLsizei countLOD3 = 0;
//
//	std::tie(vaoLOD1, vboLOD1, countLOD1) = makePointBuffer(1); // フル
//	std::tie(vaoLOD2, vboLOD2, countLOD2) = makePointBuffer(2); // 1/4
//	std::tie(vaoLOD3, vboLOD3, countLOD3) = makePointBuffer(3); // 1/9
//
//	// 現在のLODを選択（開始は軽めの LOD2 など）
//	GLuint activeVAO = vaoLOD2;
//	GLsizei activeCount = countLOD2;
//
//	// 既存の単一VAO/VBOは不要になった場合は削除してOK
//	// glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO);
//	// 以降の描画では activeVAO/activeCount を使用
//
//	//------------------------------
//
//
//	// フレーム処理
//	//------------------------------
//
//	glfwSetFramebufferSizeCallback(gridWin, framebuffer_size_callback);
//
//	long long sum_time = 0;
//	int numFrame = 0;
//	while (!glfwWindowShouldClose(gridWin))
//	{
//		// 測定開始時刻を記録
//		auto start = std::chrono::high_resolution_clock::now();
//
//		processInput(gridWin);
//
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		glEnable(GL_BLEND);
//		//ourShader.setFloat("pSize", 0.1f);
//
//		ourShader.use();
//		//glBindVertexArray(VAO);
//		// 
//		//// キーで手動切替: [1]=フル [2]=1/4 [3]=1/9
//		//if (glfwGetKey(gridWin, GLFW_KEY_1) == GLFW_PRESS) { activeVAO = vaoLOD1; activeCount = countLOD1; }
//		//if (glfwGetKey(gridWin, GLFW_KEY_2) == GLFW_PRESS) { activeVAO = vaoLOD2; activeCount = countLOD2; }
//		//if (glfwGetKey(gridWin, GLFW_KEY_3) == GLFW_PRESS) { activeVAO = vaoLOD3; activeCount = countLOD3; }
//
//		// 小さな穴を目立たせないためポイントサイズを上げる（シェーダ側未使用なら固定機能でOK）
//		glPointSize(2.0f);
//
//		// 既存: glBindVertexArray(VAO); を以下に置換
//		glBindVertexArray(activeVAO);
//
//		// pass projection matrix to shader (note that in this case it could change every frame)
//		glm::mat4 projection = glm::perspective(FOV_Y, (float)NUM_ELEM_IMG_PX_X / (float)NUM_ELEM_IMG_PX_Y, 0.1f, 100.0f);
//		ourShader.setMat4("projection", projection);
//
//		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//		ourShader.setMat4("model", model);
//
//		//// タイル描画直前
//		//GLuint q = 0; glGenQueries(1, &q);
//		//glBeginQuery(GL_TIME_ELAPSED, q);
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
//				//glClear(GL_COLOR_BUFFER_BIT);
//
//				// camera/view transformation
//				glm::mat4 view = glm::lookAt(camPos[idx], glm::vec3(camPos[idx].x, camPos[idx].y, 1.0f), glm::vec3(0.0f, 0.1f, 0.0f));
//				ourShader.setMat4("view", view);
//
//				//glDrawArrays(GL_POINTS, 0, NUM_POINTS);
//
//				// 既存: glDrawArrays(GL_POINTS, 0, NUM_POINTS); を以下に置換
//				glDrawArrays(GL_POINTS, 0, activeCount);
//			}
//		}
//		glDisable(GL_SCISSOR_TEST);
//		glDisable(GL_BLEND);
//		glBindVertexArray(0);
//		glUseProgram(0);
//
//		//glEndQuery(GL_TIME_ELAPSED);
//		//GLuint64 ns = 0; glGetQueryObjectui64v(q, GL_QUERY_RESULT, &ns);
//		//double gpuMs = ns / 1.0e6;
//		//std::printf("[GPU] tile pass = %.3f ms\n", gpuMs);
//		//glDeleteQueries(1, &q);
//
//		glfwSwapBuffers(gridWin);
//		glfwPollEvents();
//
//		// 測定終了時刻を記録
//		auto end = std::chrono::high_resolution_clock::now();
//
//		// 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//		sum_time += duration.count();
//		numFrame++;
//
//	}
//
//	cout << "フレーム数:" << numFrame << endl;
//	cout << "平均実行時間: " << sum_time / numFrame << " ms" << endl;
//
//	//------------------------------
//
//
//	// 表示画像の保存
//	//------------------------------
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
//			dst[x * 3 + 0] = src[(WIN_W - 1 - x) * 3 + 2]; // B
//			dst[x * 3 + 1] = src[(WIN_W - 1 - x) * 3 + 1]; // G
//			dst[x * 3 + 2] = src[(WIN_W - 1 - x) * 3 + 0]; // R
//		}
//	}
//
//	std::ostringstream stream;
//	stream << "D:/ForStudy/reconstruction/OpenGL-scratch-normal-v1/OpenGL-scratch-normal-v1-grid_f" << std::fixed << std::setprecision(4) << (FOCAL_LENGTH * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (SUBJECT_SIZE_X * 1000.f) << "_zi" << (int)(SUBJECT_Z * 1000.f) << ".png";
//	std::string outPath = stream.str();
//
//	cv::imwrite(outPath, img);
//
//	//------------------------------
//
//
//	// 後処理
//	//------------------------------
//
//	//glDeleteVertexArrays(1, &VAO);
//	//glDeleteBuffers(1, &VBO);
//
//	// ---- 終了処理で LOD VBO/VAO を解放 ----
//	glDeleteVertexArrays(1, &vaoLOD1); glDeleteBuffers(1, &vboLOD1);
//	glDeleteVertexArrays(1, &vaoLOD2); glDeleteBuffers(1, &vboLOD2);
//	glDeleteVertexArrays(1, &vaoLOD3); glDeleteBuffers(1, &vboLOD3);
//
//	delete[] camPos;
//	delete[] points;
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
