//// OpenGL-vt-v1.cpp
//// OpenGL-v3.cppからの派生
//// Stage1: 40×40 並行カメラ (z=0, +Z 方向) によるタイル表示
//// Stage2: 概観(Overview)カメラ。要求: 位置を z = -1.0f 上 (0,0,-1) に固定し +Z 方向を向く。
////         画角(FOV)は点群全体 (AABB, z∈[2,3]) が必ず収まるよう計算し直す。
//// 変更点: 以前の距離フィット方式を廃止し、固定位置 + 動的 FOV 計算 (各フレームでウィンドウ aspect に応じ算出)
//// FOV 計算:
////   最近点までの距離 d_near = z_min - eyeZ = 2 - (-1) = 3
////   halfYScene, halfXScene を用い
////     tan(fovY/2) >= max( halfYScene / d_near, (halfXScene / d_near) / aspect )
////   少し余裕係数 (1.05) を掛ける
//// C++17 / OpenGL 3.3 Core
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <opencv2/opencv.hpp>
//#include <vector>
//#include <string>
//#include <cmath>
//#include <cstdio>
//#include <chrono>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//struct Vec3 { float x, y, z; };
//struct Mat4 { float m[16]; };
//
//static Mat4 identity() { Mat4 r{}; for (int i = 0; i < 16; ++i) r.m[i] = (i % 5 == 0) ? 1.f : 0.f; return r; }
//static Mat4 multiply(const Mat4& a, const Mat4& b) {
//    Mat4 r{};
//    for (int c = 0; c < 4; ++c)
//        for (int ri = 0; ri < 4; ++ri)
//            r.m[c * 4 + ri] =
//            a.m[0 * 4 + ri] * b.m[c * 4 + 0] +
//            a.m[1 * 4 + ri] * b.m[c * 4 + 1] +
//            a.m[2 * 4 + ri] * b.m[c * 4 + 2] +
//            a.m[3 * 4 + ri] * b.m[c * 4 + 3];
//    return r;
//}
//static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
//static Vec3 cross(const Vec3& a, const Vec3& b) {
//    return { a.y * b.z - a.z * b.y,
//             a.z * b.x - a.x * b.z,
//             a.x * b.y - a.y * b.x };
//}
//static Vec3 normalize(const Vec3& v) {
//    float l = std::sqrt(dot(v, v)) + 1e-8f;
//    return { v.x / l,v.y / l,v.z / l };
//}
//static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
//    Vec3 f = normalize({ center.x - eye.x,center.y - eye.y,center.z - eye.z });
//    Vec3 s = normalize(cross(f, up));
//    Vec3 u = cross(s, f);
//    Mat4 r = identity();
//    r.m[0] = s.x; r.m[4] = s.y; r.m[8] = s.z;
//    r.m[1] = u.x; r.m[5] = u.y; r.m[9] = u.z;
//    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
//    r.m[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
//    r.m[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
//    r.m[14] = (f.x * eye.x + f.y * eye.y + f.z * eye.z);
//    return r;
//}
//static Mat4 perspective(float fovy, float aspect, float zN, float zF) {
//    float f = 1.f / std::tan(fovy * 0.5f);
//    Mat4 r{};
//    r.m[0] = f / aspect; r.m[5] = f;
//    r.m[10] = (zF + zN) / (zN - zF);
//    r.m[11] = -1.f;
//    r.m[14] = (2.f * zF * zN) / (zN - zF);
//    return r;
//}
//
//static GLuint compileShader(GLenum type, const char* src) {
//    GLuint sh = glCreateShader(type);
//    glShaderSource(sh, 1, &src, nullptr);
//    glCompileShader(sh);
//    GLint ok = 0; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0'); glGetShaderInfoLog(sh, len, nullptr, log.data());
//        std::fprintf(stderr, "[ShaderError]\n%s\n", log.c_str());
//        glDeleteShader(sh); return 0;
//    }
//    return sh;
//}
//static GLuint createProgram(const char* vs, const char* fs) {
//    GLuint v = compileShader(GL_VERTEX_SHADER, vs); if (!v)return 0;
//    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs); if (!f) { glDeleteShader(v); return 0; }
//    GLuint p = glCreateProgram();
//    glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
//    GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
//    if (!ok) {
//        GLint len = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
//        std::string log(len, '\0'); glGetProgramInfoLog(p, len, nullptr, log.data());
//        std::fprintf(stderr, "[LinkError]\n%s\n", log.c_str());
//        glDeleteProgram(p); p = 0;
//    }
//    glDeleteShader(v); glDeleteShader(f);
//    return p;
//}
//
//struct Point { float x, y, z, r, g, b; };
//
//static std::vector<Point> generatePointCloud(
//    int imgW, int imgH,
//	cv::Mat image,
//	float subject_position_offset,
//    float subject_pixel_pitch,
//	float subject_z
//) {
//    std::vector<Point> pts;
//    pts.reserve(size_t(imgW) * imgH);
//
//    for (int i = 0; i < imgH; ++i) {
//        for (int j = 0; j < imgW; ++j) {
//            float x = subject_position_offset + j * subject_pixel_pitch;
//            float y = subject_position_offset + i * subject_pixel_pitch;
//            float z = subject_z;
//            cv::Vec3b c = image.at<cv::Vec3b>(i, j);
//			float r = c[0];
//            float g = c[1];
//            float b = c[2];
//
//            pts.push_back({ x,y,z,r,g,b });
//        }
//    }
//
//    return pts;
//}
//
//static const char* kVS = R"(#version 330 core
//layout(location=0) in vec3 inPos;
//layout(location=1) in vec3 inCol;
//uniform mat4 uMVP;
//out vec3 vCol;
//void main(){
//    gl_Position = uMVP * vec4(inPos,1.0);
//    vCol = inCol;
//    gl_PointSize = 1.5;
//}
//)";
//static const char* kFS = R"(#version 330 core
//in vec3 vCol;
//out vec4 FragColor;
//void main(){
//    vec2 d = gl_PointCoord*2.0 - 1.0;
//    if(dot(d,d) > 1.0) discard;
//    FragColor = vec4(vCol,1.0);
//}
//)";
//
//int main() {
//    // ---- 定数 / パラメータ ----
//    const int CAM_COLS = 40, CAM_ROWS = 40;
//    const int VIEW_W = 60, VIEW_H = 60;
//    const int WIN_W = CAM_COLS * VIEW_W;
//    const int WIN_H = CAM_ROWS * VIEW_H;
//    const int PC_W = 640, PC_H = 480;
//	const float focal_length = 0.007f;
//	const float display_pixel_pitch = 13.4f * 0.0254f / std::sqrt(3840 * 3840 + 2400 * 2400); // 13.4inch 3840x2400 の場合
//    const float z_min = 0.1f, z_max = 3.0f;
//    //const float baseFovY_deg = 45.f; // Stage1 用 (固定)
//    //const float baseFovY = baseFovY_deg * 3.1415926535f / 180.f;
//    //const float tanHalf = std::tan(baseFovY * 0.5f);
//	const float tanHalf = display_pixel_pitch * VIEW_H * 0.5f / focal_length; // VIEW_H が画面高さに対応するように
//	const float baseFovY = std::atan(tanHalf) * 2.f;
//    const float occupy = 0.9f;
//
//	const int num_pinhole = CAM_COLS;  // 各軸方向のピンホール数
//    const float pinhole_array_width = 0.1804f;
//    const float pinhole_pitch = pinhole_array_width / num_pinhole;   // ピンホールピッチ
//
//    const int subject_image_resolution = 554; // 被写体の解像度
//    const float subject_size = pinhole_array_width; // 被写体のサイズ(拡大する場合 * (subz + zo_min) / zo_minを追加)
//    const float subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//    const float subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//    const float subject_z = 0.5f; // 被写体の奥行き方向の位置
//
//    if (!glfwInit()) { std::fprintf(stderr, "Failed to init GLFW\n"); return -1; }
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // ---- Stage1 (タイル表示) ----
//    GLFWwindow* gridWin = glfwCreateWindow(WIN_W, WIN_H, "Grid Cameras (Stage1)", nullptr, nullptr);
//    if (!gridWin) { std::fprintf(stderr, "Create window failed\n"); glfwTerminate(); return -1; }
//    glfwMakeContextCurrent(gridWin);
//    glfwSwapInterval(1);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::fprintf(stderr, "GLAD init failed\n"); return -1;
//    }
//
//    // カメラ配置計算
//    const float spacingX = pinhole_pitch;
//    const float spacingY = spacingX;
//    const float halfSpanX = (CAM_COLS - 1) * spacingX * 0.5f;
//    const float halfSpanY = (CAM_ROWS - 1) * spacingY * 0.5f;
//
//    std::string filenamein = "./images/standard/grid_image.png";
//    cv::Mat image_input = cv::imread(filenamein);
//
//    if (image_input.empty())
//    {
//        std::cout << "画像を開くことができませんでした。\n";
//        return -1;
//    }
//
//    cv::Mat resized_image;
//    cv::resize(image_input, resized_image, cv::Size(subject_image_resolution, subject_image_resolution), 0, 0, cv::INTER_NEAREST);
//
//    // 点群生成
//    auto points = generatePointCloud(subject_image_resolution, subject_image_resolution, resized_image, subject_position_offset, subject_pixel_pitch, subject_z);
//    std::printf("[Stage1] Point count = %zu\n", points.size());
//
//    // シェーダ
//    GLuint prog = createProgram(kVS, kFS);
//    if (!prog) return -1;
//    GLint locMVP = glGetUniformLocation(prog, "uMVP");
//
//    // VBO
//    GLuint vao = 0, vbo = 0;
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &vbo);
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//    glBindVertexArray(0);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_PROGRAM_POINT_SIZE);
//
//    struct Cam { Vec3 eye; };
//    std::vector<Cam> cams;
//    cams.reserve(CAM_COLS * CAM_ROWS);
//    for (int j = 0; j < CAM_ROWS; ++j) {
//        float y = (j - (CAM_ROWS - 1) * 0.5f) * spacingY;
//        for (int i = 0; i < CAM_COLS; ++i) {
//            float x = (i - (CAM_COLS - 1) * 0.5f) * spacingX;
//            cams.push_back({ {x,y,0.0f} });
//        }
//    }
//    const Vec3 forwardDir{ 0.f,0.f,1.f };
//    const float aspectTile = (float)VIEW_W / (float)VIEW_H;
//    auto t_start = std::chrono::high_resolution_clock::now();
//    bool measured = false;
//
//    while (!glfwWindowShouldClose(gridWin)) {
//        if (glfwGetKey(gridWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(gridWin, GLFW_TRUE);
//
//        glViewport(0, 0, WIN_W, WIN_H);
//        glClearColor(0.04f, 0.05f, 0.08f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(prog);
//        glBindVertexArray(vao);
//        Mat4 proj = perspective(baseFovY, aspectTile, 0.05f, 100.f);
//
//        glEnable(GL_SCISSOR_TEST);
//        for (int r = 0; r < CAM_ROWS; ++r) {
//            for (int c = 0; c < CAM_COLS; ++c) {
//                int idx = r * CAM_COLS + c;
//                Vec3 eye = cams[idx].eye;
//                Vec3 center = { eye.x + forwardDir.x, eye.y + forwardDir.y, eye.z + forwardDir.z };
//                int vx = c * VIEW_W;
//                int vy = (CAM_ROWS - 1 - r) * VIEW_H;
//                glViewport(vx, vy, VIEW_W, VIEW_H);
//                glScissor(vx, vy, VIEW_W, VIEW_H);
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//                Mat4 view = lookAt(eye, center, { 0.f,1.f,0.f });
//                Mat4 mvp = multiply(proj, view);
//                glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp.m);
//                glDrawArrays(GL_POINTS, 0, (GLsizei)points.size());
//            }
//        }
//        glDisable(GL_SCISSOR_TEST);
//        glBindVertexArray(0);
//        glUseProgram(0);
//
//        if (!measured) {
//            auto t_end = std::chrono::high_resolution_clock::now();
//            std::printf("[Stage1 InitElapsed] %.3f ms\n",
//                std::chrono::duration<double, std::milli>(t_end - t_start).count());
//            measured = true;
//        }
//        glfwSwapBuffers(gridWin);
//        glfwPollEvents();
//    }
//
//    glDeleteProgram(prog);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);
//    glfwDestroyWindow(gridWin);
//
//    // ---- Stage2 Overview (固定カメラ位置 (0,0,-1) + 動的FOV) ----
//    GLFWwindow* overviewWin = glfwCreateWindow(900, 700, "Overview (Stage2)", nullptr, nullptr);
//    if (!overviewWin) { glfwTerminate(); return -1; }
//    glfwMakeContextCurrent(overviewWin);
//    glfwSwapInterval(1);
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::fprintf(stderr, "GLAD init failed (overview)\n");
//        glfwDestroyWindow(overviewWin);
//        glfwTerminate();
//        return -1;
//    }
//
//    // AABB
//    Vec3 minP{ +1e9f,+1e9f,+1e9f }, maxP{ -1e9f,-1e9f,-1e9f };
//    for (const auto& p : points) {
//        if (p.x < minP.x)minP.x = p.x; if (p.y < minP.y)minP.y = p.y; if (p.z < minP.z)minP.z = p.z;
//        if (p.x > maxP.x)maxP.x = p.x; if (p.y > maxP.y)maxP.y = p.y; if (p.z > maxP.z)maxP.z = p.z;
//    }
//    Vec3 centerScene{ (minP.x + maxP.x) * 0.5f, (minP.y + maxP.y) * 0.5f, (minP.z + maxP.z) * 0.5f };
//    float halfXScene = (maxP.x - minP.x) * 0.5f;
//    float halfYScene = (maxP.y - minP.y) * 0.5f;
//    float farZ = maxP.z; // 最遠 z ≈ 3.0
//
//    // 再構築シェーダ
//    GLuint progO = createProgram(kVS, kFS);
//    if (!progO) { glfwDestroyWindow(overviewWin); glfwTerminate(); return -1; }
//    GLint locMVPO = glGetUniformLocation(progO, "uMVP");
//    GLuint vaoO = 0, vboO = 0;
//    glGenVertexArrays(1, &vaoO);
//    glGenBuffers(1, &vboO);
//    glBindVertexArray(vaoO);
//    glBindBuffer(GL_ARRAY_BUFFER, vboO);
//    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//    glBindVertexArray(0);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_PROGRAM_POINT_SIZE);
//
//    const Vec3 eyeFixed{ 0.f,0.f,-1.f }; // 要求仕様: z = -1.0f 固定
//    // 最近点距離 (eye z=-1 → 点群最小 z=2): 3
//    const float closestDepth = z_min - eyeFixed.z; // 3
//    const float farthestDepth = z_max - eyeFixed.z; // 4
//
//    while (!glfwWindowShouldClose(overviewWin)) {
//        if (glfwGetKey(overviewWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(overviewWin, GLFW_TRUE);
//
//        int w, h; glfwGetFramebufferSize(overviewWin, &w, &h);
//        if (h <= 0) { glfwPollEvents(); continue; }
//        float aspect = (float)w / (float)h;
//
//        // 必要な tan(fovY/2) を計算
//        float needTanY_vert = halfYScene / closestDepth;
//        float needTanY_horz = (halfXScene / closestDepth) / aspect;
//        float needTanY = std::max(needTanY_vert, needTanY_horz) * 1.05f; // 余裕 5%
//        if (needTanY < 0.01f) needTanY = 0.01f;
//        // 上限 (179° 程度)
//        float fovY_dyn = 2.f * std::atan(needTanY);
//        if (fovY_dyn > 3.10f) fovY_dyn = 3.10f; // ≈177.7°
//
//        glViewport(0, 0, w, h);
//        glClearColor(0.02f, 0.02f, 0.04f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(progO);
//        glBindVertexArray(vaoO);
//
//        Mat4 proj = perspective(fovY_dyn, aspect, 0.1f, farthestDepth + 10.f);
//        Mat4 view = lookAt(eyeFixed, { centerScene.x,centerScene.y, (z_min + z_max) * 0.5f }, { 0.f,1.f,0.f });
//        Mat4 mvp = multiply(proj, view);
//        glUniformMatrix4fv(locMVPO, 1, GL_FALSE, mvp.m);
//        glDrawArrays(GL_POINTS, 0, (GLsizei)points.size());
//
//        glBindVertexArray(0);
//        glUseProgram(0);
//
//        // デバッグ表示 (任意): FOV / needTan
//        // std::printf("\rFOV(deg)=%.2f      ", fovY_dyn*180.f/3.14159265f);
//
//        glfwSwapBuffers(overviewWin);
//        glfwPollEvents();
//    }
//
//    glDeleteProgram(progO);
//    glDeleteBuffers(1, &vboO);
//    glDeleteVertexArrays(1, &vaoO);
//    glfwDestroyWindow(overviewWin);
//    glfwTerminate();
//    return 0;
//}