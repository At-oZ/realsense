//// OpenGL-vt-v1からの派生
//// CPU並列処理による実装
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <opencv2/opencv.hpp>
//#include <vector>
//#include <string>
//#include <cmath>
//#include <cstdio>
//#include <chrono>
//#include <filesystem>
//#include <iostream>
//#include <thread>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
////// dGPU 要求 (Optimus / PowerXpress)
////extern "C" {
////    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
////    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
////}
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
//    return { v.x / l, v.y / l, v.z / l };
//}
//static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
//    Vec3 f = normalize({ center.x - eye.x, center.y - eye.y, center.z - eye.z });
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
//    GLuint v = compileShader(GL_VERTEX_SHADER, vs); if (!v) return 0;
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
//    int imgW, int imgH, cv::Mat image,
//    float subject_position_offset,
//    float subject_pixel_pitch,
//    float subject_z
//) {
//    std::vector<Point> pts;
//    pts.reserve(size_t(imgW) * imgH);
//    for (int i = 0; i < imgH; ++i) {
//        for (int j = 0; j < imgW; ++j) {
//            float x = subject_position_offset + j * subject_pixel_pitch;
//            float y = subject_position_offset + i * subject_pixel_pitch;
//            float z = subject_z;
//            cv::Vec3b c = image.at<cv::Vec3b>(i, j); // B G R
//            float r = c[0];
//            float g = c[1];
//            float b = c[2];
//            pts.push_back({ x,y,z,r,g,b });
//        }
//    }
//    return pts;
//}
//
//static const char* kVS = R"(#version 330 core
//layout(location=0) in vec3 inPos;
//layout(location=1) in vec3 inCol;   // B,G,R
//layout(location=2) in mat4 iMVP;    // インスタンスごとのMVP（タイルNDC変換込み）
//out vec3 vCol;
//void main(){
//    gl_Position = iMVP * vec4(inPos,1.0);
//    vCol = inCol.bgr / 255.0;
//    gl_PointSize = 1.0;
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
//static void printGLInfo(const char* tag) {
//    const GLubyte* vendor = glGetString(GL_VENDOR);
//    const GLubyte* renderer = glGetString(GL_RENDERER);
//    const GLubyte* version = glGetString(GL_VERSION);
//    const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
//    std::printf("[%s] GL_VENDOR   = %s\n", tag, vendor ? (const char*)vendor : "(null)");
//    std::printf("[%s] GL_RENDERER = %s\n", tag, renderer ? (const char*)renderer : "(null)");
//    std::printf("[%s] GL_VERSION  = %s\n", tag, version ? (const char*)version : "(null)");
//    std::printf("[%s] GLSL        = %s\n", tag, glsl ? (const char*)glsl : "(null)");
//}
//
//static GLFWmonitor* getMonitorByIndex(int index) {
//    int count = 0;
//    GLFWmonitor** mons = glfwGetMonitors(&count);
//    if (count <= 0) return nullptr;
//    if (index < 0 || index >= count) return glfwGetPrimaryMonitor();
//    return mons[index];
//}
//
//static Mat4 tileNDCTransform(int vx, int vy, int viewW, int viewH, int winW, int winH) {
//    // NDC内のスケール: タイルの占有率、平行移動: タイルの中心位置へ
//    const float sx = (float)viewW / (float)winW;
//    const float sy = (float)viewH / (float)winH;
//    const float tx = (2.0f * vx + viewW) / (float)winW - 1.0f;
//    const float ty = (2.0f * vy + viewH) / (float)winH - 1.0f;
//
//    Mat4 t = identity();
//    t.m[0]  = sx;  // xスケール
//    t.m[5]  = sy;  // yスケール
//    t.m[10] = 1.0f;
//    t.m[15] = 1.0f;
//    // clip空間で x' = sx*x + tx*w, y' = sy*y + ty*w を実現（wは維持）
//    t.m[12] = tx;  // xの平行移動（w項）
//    t.m[13] = ty;  // yの平行移動（w項）
//    return t;
//}
//
//int main() {
//    // ---- 定数 ----
//    const int CAM_COLS = 40, CAM_ROWS = 40;
//    const int VIEW_W = 60, VIEW_H = 60;
//    const int WIN_W = CAM_COLS * VIEW_W;
//    const int WIN_H = CAM_ROWS * VIEW_H;
//
//    const float pinhole_array_width = 0.1804f;
//    const int   num_pinhole = CAM_COLS;
//    const float pinhole_pitch = pinhole_array_width / num_pinhole;
//
//    const float zo_min = 1.f;
//    const float focal_length = zo_min / (3.f * (float)num_pinhole - 1.f);
//    const float display_pixel_pitch = 13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);
//    const float tanHalf = display_pixel_pitch * (VIEW_H - 1) * 0.5f / focal_length;
//    const float baseFovY = std::atan(tanHalf) * 2.f;
//
//    double intv = (focal_length + zo_min) / zo_min * pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//    int element_image_px = static_cast<int>(floor(intv)); // 要素画像の解像度
//    double display_area_size = element_image_px * num_pinhole * display_pixel_pitch; //表示画像の大きさ
//    int display_px = element_image_px * num_pinhole; // 表示画像の解像度
//
//    const float subject_z = 1.0f;
//    const int   subject_image_resolution = 554;
//    const float subject_size = display_area_size * (subject_z + zo_min) / zo_min; // 被写体のサイズ(拡大する場合 * (subz + zo_min) / zo_minを追加);
//    const float subject_pixel_pitch = subject_size / subject_image_resolution;
//    const float subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0f);
//
//    if (!glfwInit()) { std::fprintf(stderr, "Failed to init GLFW\n"); return -1; }
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // Stage1 ウィンドウ (ボーダレス + オフセット配置)
//    int targetMonitor = 0;
//    GLFWmonitor* mon = getMonitorByIndex(targetMonitor);
//    if (!mon) mon = glfwGetPrimaryMonitor();
//    const GLFWvidmode* mode = glfwGetVideoMode(mon);
//
//    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
//#ifdef GLFW_FLOATING
//    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
//#endif
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//
//    int offsetX = 120;
//    int offsetY = 60;
//    int winW = std::max(WIN_W, mode->width - offsetX);
//    int winH = std::max(WIN_H, mode->height - offsetY);
//
//    GLFWwindow* gridWin = glfwCreateWindow(winW, winH, "Grid Cameras (Stage1)", nullptr, nullptr);
//    if (!gridWin) { std::fprintf(stderr, "Create window failed\n"); glfwTerminate(); return -1; }
//    glfwMakeContextCurrent(gridWin);
//    glfwSwapInterval(1);
//
//    int mx = 0, my = 0;
//    glfwGetMonitorPos(mon, &mx, &my);
//    glfwSetWindowPos(gridWin, mx + offsetX, my + offsetY);
//#ifdef GLFW_FLOATING
//    glfwSetWindowAttrib(gridWin, GLFW_FLOATING, GLFW_TRUE);
//#endif
//    glfwShowWindow(gridWin);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::fprintf(stderr, "GLAD init failed\n"); return -1;
//    }
//    printGLInfo("Stage1");
//
//    // 画像読み込み
//    cv::Mat image_input = cv::imread("./images/standard/grid_image.png");
//    if (image_input.empty()) {
//        std::cout << "画像を開くことができませんでした。\n";
//        return -1;
//    }
//    cv::Mat resized_image;
//    cv::resize(image_input, resized_image, cv::Size(subject_image_resolution, subject_image_resolution), 0, 0, cv::INTER_NEAREST);
//
//    // 点群
//    auto points = generatePointCloud(subject_image_resolution, subject_image_resolution,
//        resized_image, subject_position_offset, subject_pixel_pitch, subject_z);
//    std::printf("[Stage1] Point count = %zu\n", points.size());
//
//    // シェーダ
//    GLuint prog = createProgram(kVS, kFS);
//    if (!prog) return -1;
//    GLint locMVP = glGetUniformLocation(prog, "uMVP");
//
//    // VAO / VBO（頂点は既存のまま）
//    GLuint vao = 0, vbo = 0;
//    glGenVertexArrays(1, &vao);
//    glGenBuffers(1, &vbo);
//    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, x));
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, r));
//    glBindVertexArray(0);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_PROGRAM_POINT_SIZE);
//
//    // カメラ配置
//    struct Cam { Vec3 eye; };
//    std::vector<Cam> cams;
//    cams.reserve(CAM_COLS * CAM_ROWS);
//    for (int j = 0; j < CAM_ROWS; ++j) {
//        float y = (j - (CAM_ROWS - 1) * 0.5f) * pinhole_pitch;
//        for (int i = 0; i < CAM_COLS; ++i) {
//            float x = (i - (CAM_COLS - 1) * 0.5f) * pinhole_pitch;
//            cams.push_back({ {x,y,0.0f} });
//        }
//    }
//    const Vec3 forwardDir{ 0.f,0.f,1.f };
//    const float aspectTile = (float)VIEW_W / (float)VIEW_H;
//
//    // 追加: タイルごとの描画ジョブ (ビューポートとMVP) を並列に構築
//    struct TileJob {
//        int vx, vy;
//        Mat4 mvp;
//    };
//    // 追加: インスタンス描画用の行列配列（タイル数分）
//    const Mat4 proj = perspective(baseFovY, aspectTile, 0.05f, 100.f);
//    std::vector<Mat4> instanceMats(CAM_ROWS * CAM_COLS);
//
//    auto buildInstanceMatsParallel = [&]() {
//        const unsigned int nt = std::max(1u, std::thread::hardware_concurrency());
//        const int rowsPerThread = (CAM_ROWS + (int)nt - 1) / (int)nt;
//
//        std::vector<std::thread> threads;
//        threads.reserve(nt);
//        for (unsigned int t = 0; t < nt; ++t) {
//            const int r0 = (int)t * rowsPerThread;
//            const int r1 = std::min(CAM_ROWS, r0 + rowsPerThread);
//            if (r0 >= r1) break;
//
//            threads.emplace_back([&, r0, r1]() {
//                for (int r = r0; r < r1; ++r) {
//                    for (int c = 0; c < CAM_COLS; ++c) {
//                        const int idx = r * CAM_COLS + c;
//                        const Vec3 eye = cams[idx].eye;
//                        const Vec3 center{ eye.x + forwardDir.x, eye.y + forwardDir.y, eye.z + forwardDir.z };
//                        const Mat4 view = lookAt(eye, center, { 0.f,-1.f,0.f });
//
//                        const int vx = c * VIEW_W;
//                        const int vy = (CAM_ROWS - 1 - r) * VIEW_H; // 上から下へ
//                        const Mat4 tile = tileNDCTransform(vx, vy, VIEW_W, VIEW_H, WIN_W, WIN_H);
//
//                        // iMVP = tile * (proj * view)
//                        instanceMats[idx] = multiply(tile, multiply(proj, view));
//                    }
//                }
//            });
//        }
//        for (auto& th : threads) th.join();
//    };
//    buildInstanceMatsParallel();
//
//    // インスタンス用行列バッファをアップロードして属性設定（mat4 は4スロットを使用: location=2..5）
//    GLuint instanceVbo = 0;
//    glBindVertexArray(vao);
//    glGenBuffers(1, &instanceVbo);
//    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
//    glBufferData(GL_ARRAY_BUFFER, instanceMats.size() * sizeof(Mat4), instanceMats.data(), GL_STATIC_DRAW);
//    for (int i = 0; i < 4; ++i) {
//        glEnableVertexAttribArray(2 + i);
//        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(sizeof(float) * 4 * i));
//        glVertexAttribDivisor(2 + i, 1);
//    }
//    glBindVertexArray(0);
//
//    auto t_start = std::chrono::high_resolution_clock::now();
//    bool measured = false;
//
//    // 全体キャプチャ (初回のみ)
//    bool savedFull = false;
//
//    while (!glfwWindowShouldClose(gridWin)) {
//        if (glfwGetKey(gridWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(gridWin, GLFW_TRUE);
//
//        // 画面全体を一度だけクリア
//        glViewport(0, 0, WIN_W, WIN_H);
//        glClearColor(0.f, 0.f, 0.f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(prog);
//        glBindVertexArray(vao);
//
//        // インスタンス描画（タイル数だけインスタンス）
//        glDrawArraysInstanced(GL_POINTS, 0, (GLsizei)points.size(), (GLsizei)instanceMats.size());
//
//        glBindVertexArray(0);
//        glUseProgram(0);
//
//        // 計測/保存は既存のまま（glReadPixels はクリアと描画の後）
//        if (!measured) {
//            auto t_end = std::chrono::high_resolution_clock::now();
//            std::printf("[Stage1 InitElapsed] %.3f ms\n",
//                std::chrono::duration<double, std::milli>(t_end - t_start).count());
//            measured = true;
//        }
//
//        // 初回フレームでタイル全体を一枚に保存
//        if (!savedFull) {
//            // WIN_W×WIN_H 領域のみ取得 (高 DPI の場合は必要に応じて調整)
//            std::vector<unsigned char> buf(WIN_W * WIN_H * 3);
//            glReadPixels(0, 0, WIN_W, WIN_H, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
//
//            cv::Mat img(WIN_H, WIN_W, CV_8UC3);
//            for (int y = 0; y < WIN_H; ++y) {
//                unsigned char* dst = img.ptr<unsigned char>(WIN_H - 1 - y);
//                const unsigned char* src = buf.data() + y * WIN_W * 3;
//                for (int x = 0; x < WIN_W; ++x) {
//                    dst[x * 3 + 0] = src[x * 3 + 2]; // B
//                    dst[x * 3 + 1] = src[x * 3 + 1]; // G
//                    dst[x * 3 + 2] = src[x * 3 + 0]; // R
//                }
//            }
//
//            std::ostringstream stream;
//            stream << "D:/ForStudy/reconstruction/OpenGL-normal-v1/OpenGL-normal-v1-grid_f" << std::fixed << std::setprecision(4) << (focal_length * 1e3) << "_subsize" << std::fixed << std::setprecision(2) << (subject_size * 1000.f) << "_zi" << (int)(subject_z * 1000.f) << "_2.png";
//            std::string outPath = stream.str();
//            if (cv::imwrite(outPath, img)) {
//                std::printf("[Stage1] Saved full image: %s (%dx%d)\n", outPath.c_str(), WIN_W, WIN_H);
//            }
//            else {
//                std::printf("[Stage1] Failed to save full image: %s\n", outPath.c_str());
//            }
//            savedFull = true;
//        }
//
//
//        glfwSwapBuffers(gridWin);
//        glfwPollEvents();
//    }
//
//    glDeleteProgram(prog);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);
//    glfwDestroyWindow(gridWin);
//
//    //// ---- Stage2 Overview ----
//    //GLFWwindow* overviewWin = glfwCreateWindow(900, 700, "Overview (Stage2)", nullptr, nullptr);
//    //if (!overviewWin) { glfwTerminate(); return -1; }
//    //glfwMakeContextCurrent(overviewWin);
//    //glfwSwapInterval(1);
//    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//    //    std::fprintf(stderr, "GLAD init failed (overview)\n");
//    //    glfwDestroyWindow(overviewWin);
//    //    glfwTerminate();
//    //    return -1;
//    //}
//    //printGLInfo("Stage2");
//
//    //// AABB
//    //Vec3 minP{ +1e9f,+1e9f,+1e9f }, maxP{ -1e9f,-1e9f,-1e9f };
//    //for (const auto& p : points) {
//    //    if (p.x < minP.x)minP.x = p.x; if (p.y < minP.y)minP.y = p.y; if (p.z < minP.z)minP.z = p.z;
//    //    if (p.x > maxP.x)maxP.x = p.x; if (p.y > maxP.y)maxP.y = p.y; if (p.z > maxP.z)maxP.z = p.z;
//    //}
//    //Vec3 centerScene{ (minP.x + maxP.x) * 0.5f, (minP.y + maxP.y) * 0.5f, (minP.z + maxP.z) * 0.5f };
//    //float halfXScene = (maxP.x - minP.x) * 0.5f;
//    //float halfYScene = (maxP.y - minP.y) * 0.5f;
//
//    //GLuint progO = createProgram(kVS, kFS);
//    //if (!progO) { glfwDestroyWindow(overviewWin); glfwTerminate(); return -1; }
//    //GLint locMVPO = glGetUniformLocation(progO, "uMVP");
//
//    //GLuint vaoO = 0, vboO = 0;
//    //glGenVertexArrays(1, &vaoO);
//    //glGenBuffers(1, &vboO);
//    //glBindVertexArray(vaoO);
//    //glBindBuffer(GL_ARRAY_BUFFER, vboO);
//    //glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
//    //glEnableVertexAttribArray(0);
//    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//    //glEnableVertexAttribArray(1);
//    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//    //glBindVertexArray(0);
//    //glEnable(GL_DEPTH_TEST);
//    //glEnable(GL_PROGRAM_POINT_SIZE);
//
//    //const Vec3 eyeFixed{ 0.f,0.f,-1.f };
//    //const float z_min = 0.1f, z_max = 3.0f;
//    //const float closestDepth = z_min - eyeFixed.z;
//    //const float farthestDepth = z_max - eyeFixed.z;
//
//    //while (!glfwWindowShouldClose(overviewWin)) {
//    //    if (glfwGetKey(overviewWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//    //        glfwSetWindowShouldClose(overviewWin, GLFW_TRUE);
//
//    //    int w, h; glfwGetFramebufferSize(overviewWin, &w, &h);
//    //    if (h <= 0) { glfwPollEvents(); continue; }
//    //    float aspect = (float)w / (float)h;
//
//    //    float needTanY_vert = halfYScene / closestDepth;
//    //    float needTanY_horz = (halfXScene / closestDepth) / aspect;
//    //    float needTanY = std::max(needTanY_vert, needTanY_horz) * 1.05f;
//    //    if (needTanY < 0.01f) needTanY = 0.01f;
//    //    float fovY_dyn = 2.f * std::atan(needTanY);
//    //    if (fovY_dyn > 3.10f) fovY_dyn = 3.10f;
//
//    //    glViewport(0, 0, w, h);
//    //    glClearColor(0.02f, 0.02f, 0.04f, 1.f);
//    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    //    glUseProgram(progO);
//    //    glBindVertexArray(vaoO);
//    //    Mat4 proj = perspective(fovY_dyn, aspect, 0.1f, farthestDepth + 10.f);
//    //    Mat4 view = lookAt(eyeFixed, { centerScene.x,centerScene.y, (z_min + z_max) * 0.5f }, { 0.f,1.f,0.f });
//    //    Mat4 mvp = multiply(proj, view);
//    //    glUniformMatrix4fv(locMVPO, 1, GL_FALSE, mvp.m);
//    //    glDrawArrays(GL_POINTS, 0, (GLsizei)points.size());
//    //    glBindVertexArray(0);
//    //    glUseProgram(0);
//
//    //    glfwSwapBuffers(overviewWin);
//    //    glfwPollEvents();
//    //}
//
//    //glDeleteProgram(progO);
//    //glDeleteBuffers(1, &vboO);
//    //glDeleteVertexArrays(1, &vaoO);
//    //glfwDestroyWindow(overviewWin);
//
//    glfwTerminate();
//    return 0;
//}