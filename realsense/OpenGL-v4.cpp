//// OpenGL-v4.cpp
//// OpenGL学習用プログラム：点の生成、カメラ撮影、ウィンドウ表示
//// 目標：一つの点を生成し、カメラで撮影して表示する
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <vector>
//#include <iostream>
//#include <cmath>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//// 3D座標とカラー情報を持つ点の構造体
//struct Point {
//    float x, y, z;    // 3D座標
//    float r, g, b;    // RGB色情報
//};
//
//// 4x4行列構造体（行列演算用）
//struct Mat4 {
//    float m[16];
//};
//
//// 3Dベクトル構造体
//struct Vec3 {
//    float x, y, z;
//};
//
//// 単位行列を作成
//Mat4 identity() {
//    Mat4 result = {};
//    for (int i = 0; i < 16; ++i) {
//        result.m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
//    }
//    return result;
//}
//
//// 行列の乗算
//Mat4 multiply(const Mat4& a, const Mat4& b) {
//    Mat4 result = {};
//    for (int col = 0; col < 4; ++col) {
//        for (int row = 0; row < 4; ++row) {
//            result.m[col * 4 + row] = 
//                a.m[0 * 4 + row] * b.m[col * 4 + 0] +
//                a.m[1 * 4 + row] * b.m[col * 4 + 1] +
//                a.m[2 * 4 + row] * b.m[col * 4 + 2] +
//                a.m[3 * 4 + row] * b.m[col * 4 + 3];
//        }
//    }
//    return result;
//}
//
//// ベクトルの内積
//float dot(const Vec3& a, const Vec3& b) {
//    return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//
//// ベクトルの外積
//Vec3 cross(const Vec3& a, const Vec3& b) {
//    return {
//        a.y * b.z - a.z * b.y,
//        a.z * b.x - a.x * b.z,
//        a.x * b.y - a.y * b.x
//    };
//}
//
//// ベクトルの正規化
//Vec3 normalize(const Vec3& v) {
//    float length = std::sqrt(dot(v, v)) + 1e-8f;
//    return { v.x / length, v.y / length, v.z / length };
//}
//
//// ビュー行列の作成（カメラの位置と向きを定義）
//Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
//    Vec3 forward = normalize({ center.x - eye.x, center.y - eye.y, center.z - eye.z });
//    Vec3 side = normalize(cross(forward, up));
//    Vec3 upward = cross(side, forward);
//    
//    Mat4 result = identity();
//    result.m[0] = side.x;    result.m[4] = side.y;    result.m[8] = side.z;
//    result.m[1] = upward.x;  result.m[5] = upward.y;  result.m[9] = upward.z;
//    result.m[2] = -forward.x; result.m[6] = -forward.y; result.m[10] = -forward.z;
//    
//    result.m[12] = -(side.x * eye.x + side.y * eye.y + side.z * eye.z);
//    result.m[13] = -(upward.x * eye.x + upward.y * eye.y + upward.z * eye.z);
//    result.m[14] = (forward.x * eye.x + forward.y * eye.y + forward.z * eye.z);
//    
//    return result;
//}
//
//// 透視投影行列の作成（遠近感を表現）
//Mat4 perspective(float fovY, float aspect, float nearPlane, float farPlane) {
//    float f = 1.0f / std::tan(fovY * 0.5f);
//    Mat4 result = {};
//    
//    result.m[0] = f / aspect;
//    result.m[5] = f;
//    result.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
//    result.m[11] = -1.0f;
//    result.m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
//    
//    return result;
//}
//
//// シェーダーのコンパイル
//GLuint compileShader(GLenum type, const char* source) {
//    GLuint shader = glCreateShader(type);
//    glShaderSource(shader, 1, &source, nullptr);
//    glCompileShader(shader);
//    
//    GLint success = 0;
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        GLint logLength = 0;
//        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
//        std::vector<char> log(logLength);
//        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
//        std::cout << "シェーダーコンパイルエラー: " << log.data() << std::endl;
//        glDeleteShader(shader);
//        return 0;
//    }
//    
//    return shader;
//}
//
//// シェーダープログラムの作成
//GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
//    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
//    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
//    
//    if (!vertexShader || !fragmentShader) {
//        return 0;
//    }
//    
//    GLuint program = glCreateProgram();
//    glAttachShader(program, vertexShader);
//    glAttachShader(program, fragmentShader);
//    glLinkProgram(program);
//    
//    GLint success = 0;
//    glGetProgramiv(program, GL_LINK_STATUS, &success);
//    if (!success) {
//        GLint logLength = 0;
//        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
//        std::vector<char> log(logLength);
//        glGetProgramInfoLog(program, logLength, nullptr, log.data());
//        std::cout << "シェーダーリンクエラー: " << log.data() << std::endl;
//        glDeleteProgram(program);
//        program = 0;
//    }
//    
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//    
//    return program;
//}
//
//// 頂点シェーダー（3D座標を2D画面座標に変換）
//const char* vertexShaderSource = R"(
//#version 330 core
//layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 color;
//
//uniform mat4 mvp;  // Model-View-Projection matrix
//
//out vec3 vertexColor;
//
//void main() {
//    gl_Position = mvp * vec4(position, 1.0);
//    vertexColor = color;
//    gl_PointSize = 10.0;  // 点のサイズを10ピクセルに設定
//}
//)";
//
//// フラグメントシェーダー（ピクセルの色を決定）
//const char* fragmentShaderSource = R"(
//#version 330 core
//in vec3 vertexColor;
//out vec4 FragColor;
//
//void main() {
//    // 点を円形にする（円の外側を透明にする）
//    vec2 coord = gl_PointCoord * 2.0 - 1.0;
//    if (dot(coord, coord) > 1.0) {
//        discard;  // 円の外側のピクセルを破棄
//    }
//    
//    FragColor = vec4(vertexColor, 1.0);
//}
//)";
//
//// エラーコールバック関数
//void errorCallback(int error, const char* description) {
//    std::cout << "GLFWエラー " << error << ": " << description << std::endl;
//}
//
//// ウィンドウサイズ変更コールバック
//void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
//    glViewport(0, 0, width, height);
//}
//
//int main() {
//    std::cout << "OpenGL学習プログラム：点の生成と表示" << std::endl;
//    
//    // GLFWの初期化
//    glfwSetErrorCallback(errorCallback);
//    if (!glfwInit()) {
//        std::cout << "GLFWの初期化に失敗しました" << std::endl;
//        return -1;
//    }
//    
//    // OpenGLのバージョン設定（3.3 Core Profile）
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    
//    // ウィンドウの作成
//    const int WINDOW_WIDTH = 800;
//    const int WINDOW_HEIGHT = 600;
//    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
//                                         "OpenGL点群表示", nullptr, nullptr);
//    if (!window) {
//        std::cout << "ウィンドウの作成に失敗しました" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    
//    /* ここから処理開始 */
//    // OpenGLコンテキストを現在のスレッドに設定
//    glfwMakeContextCurrent(window); // 以下の処理を行うウィンドウを設定（コンテキスト）
//	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback); // ウィンドウサイズ変更
//    
//    // GLADの初期化（OpenGL関数ポインタの読み込み）
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cout << "GLADの初期化に失敗しました" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    
//    std::cout << "OpenGLバージョン: " << glGetString(GL_VERSION) << std::endl;
//    
//    // 1つの点を生成（3D空間の原点から少し離れた位置）
//    std::vector<Point> points = {
//        { 0.0f, 0.0f, -2.0f,    1.0f, 0.0f, 0.0f },  // 赤い点をz=-2の位置に配置
//        { 0.0f, 0.1f, -2.0f,    0.0f, 1.0f, 0.0f },
//        { 0.1f, 0.0f, -2.0f,    0.0f, 0.0f, 1.0f },
//        { 0.1f, 0.1f, -2.0f,    1.0f, 1.0f, 0.0f }
//    };
//    
//    std::cout << "生成された点の数: " << points.size() << std::endl;
//    std::cout << "点の座標: (" << points[0].x << ", " << points[0].y << ", " << points[0].z << ")" << std::endl;
//    
//    // シェーダープログラムの作成
//    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
//    if (!shaderProgram) {
//        std::cout << "シェーダープログラムの作成に失敗しました" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    
//    // VAO（Vertex Array Object）とVBO（Vertex Buffer Object）の作成
//    GLuint VAO, VBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    
//    // VAOをバインド
//    glBindVertexArray(VAO);
//    
//    // VBOに点のデータを送信
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    
//    // 頂点属性の設定
//    // 位置属性（location = 0）
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//    glEnableVertexAttribArray(0);
//    
//    // 色属性（location = 1）
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 
//                         (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//    
//    // OpenGLの設定
//    glEnable(GL_DEPTH_TEST);        // 深度テストを有効化
//    glEnable(GL_PROGRAM_POINT_SIZE); // プログラムで点のサイズを制御
//    
//    // カメラパラメータの設定
//    Vec3 cameraPos = { 0.0f, 0.0f, 0.0f };      // カメラ位置
//    Vec3 targetPos = { 0.0f, 0.0f, -2.0f };     // カメラが見る方向（点の位置）
//    Vec3 upVector = { 0.0f, 1.0f, 0.0f };       // カメラの上方向
//    
//    std::cout << "カメラ設定:" << std::endl;
//    std::cout << "  位置: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
//    std::cout << "  視点: (" << targetPos.x << ", " << targetPos.y << ", " << targetPos.z << ")" << std::endl;
//    
//    // メインループ
//    while (!glfwWindowShouldClose(window)) {
//        // ESCキーでプログラム終了
//        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//            glfwSetWindowShouldClose(window, GLFW_TRUE);
//        }
//        
//        // 画面をクリア
//        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // 暗い青色の背景
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        
//        // ウィンドウサイズを取得してアスペクト比を計算
//        int width, height;
//        glfwGetFramebufferSize(window, &width, &height);
//        float aspectRatio = (float)width / (float)height;
//        
//        // 投影行列とビュー行列を作成
//        Mat4 projection = perspective(45.0f * 3.14159f / 180.0f, aspectRatio, 0.1f, 100.0f);
//        Mat4 view = lookAt(cameraPos, targetPos, upVector);
//        Mat4 model = identity();  // モデル行列（今回は単位行列）
//        
//        // MVP行列（Model-View-Projection）を計算
//        Mat4 mvp = multiply(projection, multiply(view, model));
//        
//        // シェーダープログラムを使用
//        glUseProgram(shaderProgram);
//        
//        // MVP行列をシェーダーに送信
//        GLint mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
//        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.m);
//        
//        // 点を描画
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
//        
//        // バッファを交換
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//    
//    // リソースのクリーンアップ
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteProgram(shaderProgram);
//    
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    
//    std::cout << "プログラムが正常に終了しました" << std::endl;
//    return 0;
//}