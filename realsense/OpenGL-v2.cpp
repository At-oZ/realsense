//// OpenGL-v2.cpp
//// 適当な点群データを生成し、仮想カメラ視点から描画するサンプル (C++17 / OpenGL 3.3 Core)
//// 依存ライブラリ:
////   - GLFW (ウィンドウ & 入力) https://www.glfw.org/
////   - GLAD (OpenGL ローダ)     https://glad.dav1d.de/  (API: OpenGL 3.3 Core, C/C++)
//// ビルド例 (vcpkg 利用):
////   vcpkg install glfw3
////   GLAD は生成した glad.c をプロジェクトに追加し、include パスを設定
////
//// 操作:
////   マウス右ドラッグ: カメラ回転
////   WASD / QE:        平行移動
////   マウスホイール:   視点距離(前後)
////   ESC:              終了
////
//// 目的:
////   仮想カメラ行列 (View, Projection) と点群(VBO) + シェーダ(色付与) の最小構成例
//
//#include <cstdio>
//#include <cstdlib>
//#include <vector>
//#include <string>
//#include <cmath>
//#include <chrono>
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//// 後方互換 (古い GLFW を拾っている場合の一時対応)
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//struct Vec3 {
//    float x, y, z;
//};
//
//struct Mat4 {
//    float m[16]; // column-major
//};
//
//static Mat4 identity() {
//    Mat4 r{};
//    for (int i = 0; i < 16; ++i) r.m[i] = (i % 5 == 0) ? 1.f : 0.f;
//    return r;
//}
//
//static Mat4 multiply(const Mat4& a, const Mat4& b) {
//    Mat4 r{};
//    for (int c = 0; c < 4; ++c)
//        for (int rIdx = 0; rIdx < 4; ++rIdx) {
//            r.m[c * 4 + rIdx] =
//                a.m[0 * 4 + rIdx] * b.m[c * 4 + 0] +
//                a.m[1 * 4 + rIdx] * b.m[c * 4 + 1] +
//                a.m[2 * 4 + rIdx] * b.m[c * 4 + 2] +
//                a.m[3 * 4 + rIdx] * b.m[c * 4 + 3];
//        }
//    return r;
//}
//
//static Mat4 translate(const Vec3& v) {
//    Mat4 r = identity();
//    r.m[12] = v.x; r.m[13] = v.y; r.m[14] = v.z;
//    return r;
//}
//
//static Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) {
//    float f = 1.f / std::tan(fovyRad / 2.f);
//    Mat4 r{};
//    r.m[0] = f / aspect;
//    r.m[5] = f;
//    r.m[10] = (zFar + zNear) / (zNear - zFar);
//    r.m[11] = -1.f;
//    r.m[14] = (2.f * zFar * zNear) / (zNear - zFar);
//    return r;
//}
//
//static Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
//static Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
//static Vec3 operator*(const Vec3& a, float s) { return {a.x * s, a.y * s, a.z * s}; }
//
//static Vec3 normalize(const Vec3& v) {
//    float l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z) + 1e-8f;
//    return {v.x / l, v.y / l, v.z / l};
//}
//
//static Vec3 cross(const Vec3& a, const Vec3& b) {
//    return {
//        a.y * b.z - a.z * b.y,
//        a.z * b.x - a.x * b.z,
//        a.x * b.y - a.y * b.x
//    };
//}
//
//static float dot(const Vec3& a, const Vec3& b) {
//    return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//
//static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
//    Vec3 f = normalize(center - eye);
//    Vec3 s = normalize(cross(f, up));
//    Vec3 u = cross(s, f);
//    Mat4 r = identity();
//    r.m[0] = s.x; r.m[4] = s.y; r.m[8]  = s.z;
//    r.m[1] = u.x; r.m[5] = u.y; r.m[9]  = u.z;
//    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
//    r.m[12] = -dot(s, eye);
//    r.m[13] = -dot(u, eye);
//    r.m[14] = dot(f, eye);
//    return r;
//}
//
//// -------------------------------------- シェーダ --------------------------------------
//static GLuint compileShader(GLenum type, const char* src) {
//    GLuint sh = glCreateShader(type);
//    glShaderSource(sh, 1, &src, nullptr);
//    glCompileShader(sh);
//    GLint ok = 0;
//    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0;
//        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0');
//        glGetShaderInfoLog(sh, len, nullptr, log.data());
//        std::fprintf(stderr, "[Shader Compile Error]\n%s\n", log.c_str());
//        glDeleteShader(sh);
//        return 0;
//    }
//    return sh;
//}
//
//static GLuint createProgram(const char* vs, const char* fs) {
//    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
//    if (!v) return 0;
//    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
//    if (!f) { glDeleteShader(v); return 0; }
//    GLuint prog = glCreateProgram();
//    glAttachShader(prog, v);
//    glAttachShader(prog, f);
//    glLinkProgram(prog);
//    GLint ok = 0;
//    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0;
//        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0');
//        glGetProgramInfoLog(prog, len, nullptr, log.data());
//        std::fprintf(stderr, "[Program Link Error]\n%s\n", log.c_str());
//        glDeleteProgram(prog);
//        prog = 0;
//    }
//    glDeleteShader(v);
//    glDeleteShader(f);
//    return prog;
//}
//
//// -------------------------------------- 点群生成 --------------------------------------
//// 格子 (x,z) 上に高さ y = sin(x)*cos(z) の波面 + らせんを追加
//struct Point {
//    float x, y, z;
//    float r, g, b;
//};
//
//static std::vector<Point> generatePointCloud() {
//    std::vector<Point> pts;
//    const int N = 160;
//    float half = 5.0f;
//    for (int i = 0; i < N; ++i) {
//        float x = ((float)i / (N - 1) * 2.f - 1.f) * half;
//        for (int j = 0; j < N; ++j) {
//            float z = ((float)j / (N - 1) * 2.f - 1.f) * half;
//            float y = std::sin(x * 0.7f) * std::cos(z * 0.7f) * 0.8f;
//            float dist = std::sqrt(x * x + z * z);
//            float h = (y + 0.8f) / 1.6f;
//            // 色: 高さ+距離でグラデーション
//            float r = 0.3f + 0.7f * h;
//            float g = 0.2f + 0.5f * std::exp(-dist * 0.15f);
//            float b = 0.9f - 0.7f * h;
//            pts.push_back({x, y, z, r, g, b});
//        }
//    }
//    // らせん
//    for (int k = 0; k < 2000; ++k) {
//        float t = k * 0.015f;
//        float x = std::cos(t) * 1.0f;
//        float z = std::sin(t) * 1.0f;
//        float y = (k / 2000.f) * 3.f - 1.5f;
//        float r = 1.0f, g = 0.85f - 0.4f * (k / 2000.f), b = 0.1f + 0.4f * (k / 2000.f);
//        pts.push_back({x, y, z, r, g, b});
//    }
//    return pts;
//}
//
//// -------------------------------------- 入力管理 --------------------------------------
//struct Camera {
//    float yaw = 0.0f;    // 水平角
//    float pitch = 0.2f;  // 垂直角
//    float distance = 10.0f;
//    Vec3 target{0.f, 0.f, 0.f};
//    Vec3 pan{0.f, 0.f, 0.f}; // 平行移動
//};
//
//static bool g_mouseRot = false;
//static double g_prevX = 0.0, g_prevY = 0.0;
//
//static void cursorPosCB(GLFWwindow* win, double x, double y) {
//    Camera* cam = (Camera*)glfwGetWindowUserPointer(win);
//    if (g_mouseRot) {
//        double dx = x - g_prevX;
//        double dy = y - g_prevY;
//        cam->yaw += (float)dx * 0.005f;
//        cam->pitch += (float)dy * 0.005f;
//        if (cam->pitch > 1.5f) cam->pitch = 1.5f;
//        if (cam->pitch < -1.5f) cam->pitch = -1.5f;
//    }
//    g_prevX = x; g_prevY = y;
//}
//
//static void mouseBtnCB(GLFWwindow* win, int button, int action, int mods) {
//    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        g_mouseRot = (action == GLFW_PRESS);
//    }
//}
//
//static void scrollCB(GLFWwindow* win, double /*xoff*/, double yoff) {
//    Camera* cam = (Camera*)glfwGetWindowUserPointer(win);
//    cam->distance *= (float)std::pow(0.9, yoff);
//    if (cam->distance < 1.0f) cam->distance = 1.0f;
//    if (cam->distance > 200.f) cam->distance = 200.f;
//}
//
//static void processKeyboard(GLFWwindow* win, Camera& cam, float dt) {
//    float speed = 5.0f * dt * (cam.distance * 0.1f);
//    if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed *= 4.f;
//    // カメラの水平前方と右ベクトル
//    Vec3 forward{ std::sin(cam.yaw), 0.f, -std::cos(cam.yaw) };
//    Vec3 right{ forward.z, 0.f, -forward.x };
//    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) cam.pan = cam.pan + forward * speed;
//    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) cam.pan = cam.pan - forward * speed;
//    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) cam.pan = cam.pan + right * speed;
//    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) cam.pan = cam.pan - right * speed;
//    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) cam.pan.y += speed;
//    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) cam.pan.y -= speed;
//}
//
//// -------------------------------------- メイン --------------------------------------
//int main() {
//    if (!glfwInit()) {
//        std::fprintf(stderr, "Failed to init GLFW\n");
//        return -1;
//    }
//
//    int maj, min, rev;
//    glfwGetVersion(&maj, &min, &rev);
//    std::printf("GLFW runtime version: %d.%d.%d\n", maj, min, rev);
//    std::printf("Expecting 3.x. If 2.x -> update / include path check.\n");
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//#ifdef _WIN32
//    // Windows ではこれで OK
//#else
//    // macOS なら: glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    GLFWwindow* window = glfwCreateWindow(1280, 720, "Point Cloud Sample", nullptr, nullptr);
//    if (!window) {
//        std::fprintf(stderr, "Failed to create window\n");
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::fprintf(stderr, "Failed to init GLAD\n");
//        return -1;
//    }
//
//    Camera cam;
//    glfwSetWindowUserPointer(window, &cam);
//    glfwSetCursorPosCallback(window, cursorPosCB);
//    glfwSetMouseButtonCallback(window, mouseBtnCB);
//    glfwSetScrollCallback(window, scrollCB);
//
//    auto points = generatePointCloud();
//
//    GLuint vao = 0, vbo = 0;
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &vbo);
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    // layout(location=0) position (vec3)
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//    // layout(location=1) color (vec3)
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//    glBindVertexArray(0);
//
//    const char* vs = R"(#version 330 core
//layout(location=0) in vec3 inPos;
//layout(location=1) in vec3 inCol;
//uniform mat4 uMVP;
//out vec3 vCol;
//void main(){
//    gl_Position = uMVP * vec4(inPos, 1.0);
//    vCol = inCol;
//    gl_PointSize = 3.0;
//}
//)";
//
//    const char* fs = R"(#version 330 core
//in vec3 vCol;
//out vec4 FragColor;
//void main(){
//    // 円形ポイント (GL_POINT_SPRITE 代替・アルファ形状)
//    vec2 c = gl_PointCoord * 2.0 - 1.0;
//    float r2 = dot(c,c);
//    if(r2 > 1.0) discard;
//    float falloff = smoothstep(1.0, 0.0, r2);
//    FragColor = vec4(vCol * falloff, 1.0);
//}
//)";
//
//    GLuint prog = createProgram(vs, fs);
//    if (!prog) {
//        std::fprintf(stderr, "Failed to create shader program\n");
//        return -1;
//    }
//
//    GLint locMVP = glGetUniformLocation(prog, "uMVP");
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_PROGRAM_POINT_SIZE);
//
//    auto lastTime = std::chrono::high_resolution_clock::now();
//
//    while (!glfwWindowShouldClose(window)) {
//        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//            // ここで GLFW_TRUE が未定義なら古いヘッダ
//            glfwSetWindowShouldClose(window, GLFW_TRUE);
//        }
//
//        auto now = std::chrono::high_resolution_clock::now();
//        float dt = std::chrono::duration<float>(now - lastTime).count();
//        lastTime = now;
//
//        processKeyboard(window, cam, dt);
//
//        int w, h;
//        glfwGetFramebufferSize(window, &w, &h);
//        float aspect = (h == 0) ? 1.f : (float)w / (float)h;
//
//        // カメラ位置 (球面座標)
//        Vec3 dir{
//            std::sin(cam.yaw) * std::cos(cam.pitch),
//            std::sin(cam.pitch),
//            -std::cos(cam.yaw) * std::cos(cam.pitch)
//        };
//        Vec3 eye = cam.target + cam.pan - dir * cam.distance;
//        Mat4 view = lookAt(eye, cam.target + cam.pan, {0.f, 1.f, 0.f});
//        Mat4 proj = perspective(45.f * 3.1415926f / 180.f, aspect, 0.05f, 500.f);
//        Mat4 mvp = multiply(proj, view);
//
//        glViewport(0, 0, w, h);
//        glClearColor(0.05f, 0.07f, 0.1f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(prog);
//        glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp.m);
//        glBindVertexArray(vao);
//        glDrawArrays(GL_POINTS, 0, (GLsizei)points.size());
//        glBindVertexArray(0);
//        glUseProgram(0);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glDeleteProgram(prog);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}