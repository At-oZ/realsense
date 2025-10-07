//// OpenGL-vt-v2-trial.cpp
//// 改善版: VBO/PBO ストリーミング最適化 + CPU 並列化 + 行列事前計算
//// C++17 / OpenGL 3.3 Core (with optional ARB_buffer_storage)
//
//#include <librealsense2/rs.hpp>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <opencv2/opencv.hpp>
//#include <algorithm>
//#include <vector>
//#include <iostream>
//#include <chrono>
//#include <cmath>
//#include <cstdio>
//#include <thread>
//#include <cstring> // memcpy
//#include <atomic>
//
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//// dGPU 要求 (Optimus / PowerXpress)
//extern "C" {
//    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}
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
//                a.m[0 * 4 + ri] * b.m[c * 4 + 0] +
//                a.m[1 * 4 + ri] * b.m[c * 4 + 1] +
//                a.m[2 * 4 + ri] * b.m[c * 4 + 2] +
//                a.m[3 * 4 + ri] * b.m[c * 4 + 3];
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
//// 頂点: 位置 + テクスチャ座標 (RealSense color frame を GPU でサンプル)
//#pragma pack(push,1)
//struct Point {
//    float x, y, z;
//    float u, v;   // 0..1
//};
//#pragma pack(pop)
//
//// 頂点/フラグメントシェーダ
//static const char* kVS = R"(#version 330 core
//layout(location=0) in vec3 inPos;
//layout(location=1) in vec2 inUV;
//
//uniform mat4 uMVP;
//out vec2 vUV;
//void main(){
//    gl_Position = uMVP * vec4(inPos,1.0);
//    vUV = inUV;
//    gl_PointSize = 1.0;
//}
//)";
//
//static const char* kFS = R"(#version 330 core
//in vec2 vUV;
//out vec4 FragColor;
//uniform sampler2D uColorTex;
//
//void main(){
//    // 円形ポイント (必要なければ外周 discard を外す)
//    vec2 d = gl_PointCoord*2.0 - 1.0;
//    //if(dot(d,d) > 1.0) discard;
//
//    // RealSense BGR8 をアップロード時に FORMAT=GL_BGR 指定しているのでそのまま
//    vec3 col = texture(uColorTex, vUV).rgb;
//    FragColor = vec4(col,1.0);
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
//int main() {
//    // ---- 固定パラメータ ----
//    const int CAM_COLS = 128, CAM_ROWS = 40;
//    const int VIEW_W = 60, VIEW_H = 60;
//    const int WIN_W = CAM_COLS * VIEW_W;
//    const int WIN_H = CAM_ROWS * VIEW_H;
//    int PC_W = 640, PC_H = 480;
//    const int FPS = 30;
//    const float focal_length = 0.0084f;
//    const float display_pixel_pitch =
//        13.4f * 0.0254f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);
//    const float D = 0.196f;
//
//    if (!glfwInit()) { std::fprintf(stderr, "GLFW init fail\n"); return -1; }
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
//    int offsetX = 2560;
//    int offsetY = 0;
//    int winW = std::max(WIN_W, mode->width - offsetX);
//    int winH = std::max(WIN_H, mode->height - offsetY);
//
//    GLFWwindow* win = glfwCreateWindow(WIN_W, WIN_H, "Grid Cameras (Optimized)", nullptr, nullptr);
//    if (!win) { glfwTerminate(); return -1; }
//    glfwMakeContextCurrent(win);
//    glfwSwapInterval(0); // VSync 無効化（計測安定化）
//
//    int mx = 0, my = 0;
//    glfwGetMonitorPos(mon, &mx, &my);
//    glfwSetWindowPos(win, mx + offsetX, my + offsetY);
//#ifdef GLFW_FLOATING
//    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
//#endif
//    glfwShowWindow(win);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::fprintf(stderr, "GLAD fail\n"); return -1;
//    }
//    printGLInfo("Stage1");
//
//    // sRGB (必要に応じ切替)
//    glEnable(GL_FRAMEBUFFER_SRGB);
//
//    // ---- RealSense ----
//    rs2::config cfg;
//    cfg.enable_stream(RS2_STREAM_COLOR, PC_W, PC_H, RS2_FORMAT_BGR8, FPS);
//    cfg.enable_stream(RS2_STREAM_DEPTH, PC_W, PC_H, RS2_FORMAT_Z16, FPS);
//    rs2::pipeline pipe;
//    pipe.start(cfg);
//    rs2::align align_to_color(RS2_STREAM_COLOR);
//    rs2::pointcloud pc;
//    rs2::points rs_points;
//
//    // ---- 仮想カメラ配置 ----
//    const float pinhole_array_width = 0.1804f;
//    const float pinhole_pitch = pinhole_array_width / CAM_COLS;
//    struct Cam { Vec3 eye; };
//    std::vector<Cam> cams; cams.reserve(CAM_COLS * CAM_ROWS);
//    for (int j = 0; j < CAM_ROWS; ++j) {
//        float y = -(j - (CAM_ROWS - 1) * 0.5f) * pinhole_pitch;
//        for (int i = 0; i < CAM_COLS; ++i) {
//            float x = -(i - (CAM_COLS - 1) * 0.5f) * pinhole_pitch;
//            cams.push_back({ {x,y,0.f} });
//        }
//    }
//    const Vec3 forwardDir{ 0,0,1 };
//    const float aspectTile = (float)VIEW_W / (float)VIEW_H;
//    const float tanHalf = display_pixel_pitch * VIEW_H * 0.5f / focal_length;
//    const float baseFovY = std::atan(tanHalf) * 2.f;
//    Mat4 proj = perspective(baseFovY, aspectTile, 0.05f, 100.f);
//
//    // ---- タイルごとの MVP/Viewport を事前計算（並列） ----
//    const size_t TILE_COUNT = size_t(CAM_COLS) * size_t(CAM_ROWS);
//    std::vector<Mat4> tileMVP(TILE_COUNT);
//    std::vector<int> tileVX(TILE_COUNT), tileVY(TILE_COUNT);
//    auto recomputeTileData = [&]() {
//        unsigned tc = std::max(1u, std::thread::hardware_concurrency());
//        std::vector<std::thread> threads; threads.reserve(tc);
//        auto worker = [&](size_t begin, size_t end) {
//            for (size_t idx = begin; idx < end; ++idx) {
//                int r = int(idx / CAM_COLS);
//                int c = int(idx % CAM_COLS);
//                const Vec3& eye = cams[idx].eye;
//                Vec3 ctr{ eye.x + forwardDir.x, eye.y + forwardDir.y, eye.z + forwardDir.z };
//                Mat4 view = lookAt(eye, ctr, { 0,1,0 });
//                tileMVP[idx] = multiply(proj, view);
//                tileVX[idx] = c * VIEW_W;
//                tileVY[idx] = (CAM_ROWS - 1 - r) * VIEW_H;
//            }
//        };
//        size_t chunk = (TILE_COUNT + tc - 1) / tc;
//        for (unsigned t = 0; t < tc; ++t) {
//            size_t b = size_t(t) * chunk;
//            size_t e = std::min(TILE_COUNT, b + chunk);
//            if (b < e) threads.emplace_back(worker, b, e);
//        }
//        for (auto& th : threads) th.join();
//    };
//    recomputeTileData();
//
//    // ---- GL リソース ----
//    GLuint prog = createProgram(kVS, kFS);
//    if (!prog) return -1;
//    GLint locMVP = glGetUniformLocation(prog, "uMVP");
//    GLint locColor = glGetUniformLocation(prog, "uColorTex");
//
//    GLuint vao = 0;
//    glGenVertexArrays(1, &vao);
//
//    // ---- 頂点バッファ（VBO）ストリーミング設定 ----
//    const size_t max_points = size_t(PC_W) * size_t(PC_H);
//    const size_t slotPoints = max_points;
//    const size_t slotSize = slotPoints * sizeof(Point);
//
//    const bool hasBufferStorage = (GLAD_GL_VERSION_4_4 != 0);
//    // 永続リング: 3 スロット。フォールバック: 3 本の VBO をローテーション
//    const int NUM_SLOTS = 3;
//
//    // 永続マップ用
//    GLuint vbo_ring = 0;
//    Point* ringMapped = nullptr;
//    std::vector<GLsync> vboFences(NUM_SLOTS, 0);
//
//    // フォールバック用
//    GLuint vbos[NUM_SLOTS] = { 0,0,0 };
//
//    if (hasBufferStorage) {
//        glGenBuffers(1, &vbo_ring);
//        glBindVertexArray(vao);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo_ring);
//        GLbitfield storageFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
//        glBufferStorage(GL_ARRAY_BUFFER, slotSize * NUM_SLOTS, nullptr, storageFlags);
//        ringMapped = (Point*)glMapBufferRange(GL_ARRAY_BUFFER, 0, slotSize * NUM_SLOTS, storageFlags);
//        // VAO のレイアウトはフレーム毎にオフセットを更新
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//        glBindVertexArray(0);
//    } else {
//        glGenBuffers(NUM_SLOTS, vbos);
//        glBindVertexArray(vao);
//        for (int i = 0; i < NUM_SLOTS; ++i) {
//            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
//            glBufferData(GL_ARRAY_BUFFER, slotSize, nullptr, GL_STREAM_DRAW);
//        }
//        // 初期は 0 番 VBO を VAO に関連付け
//        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//        glBindVertexArray(0);
//    }
//
//    // ---- カラーフレーム用テクスチャ + PBO（非同期アップロード） ----
//    GLuint texColor = 0;
//    glGenTextures(1, &texColor);
//    glBindTexture(GL_TEXTURE_2D, texColor);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, PC_W, PC_H, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    const size_t colorBytes = size_t(PC_W) * size_t(PC_H) * 3;
//    const int NUM_PBO = 2;
//    GLuint pbo[NUM_PBO] = { 0,0 };
//    GLsync pboFence[NUM_PBO] = { 0,0 };
//
//    if (hasBufferStorage) {
//        glGenBuffers(NUM_PBO, pbo);
//        for (int i = 0; i < NUM_PBO; ++i) {
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
//            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
//            glBufferStorage(GL_PIXEL_UNPACK_BUFFER, colorBytes, nullptr, flags);
//            // 事前マップ（永続）
//            // glMapBufferRange はここでは使わず、毎回 glBufferSubData しないため、後で map して memcpy する
//        }
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//    } else {
//        glGenBuffers(NUM_PBO, pbo);
//        for (int i = 0; i < NUM_PBO; ++i) {
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
//            glBufferData(GL_PIXEL_UNPACK_BUFFER, colorBytes, nullptr, GL_STREAM_DRAW);
//        }
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//    }
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_PROGRAM_POINT_SIZE);
//
//    size_t pointCount = 0;
//
//    // FPS 計測
//    int frameNum = 0;
//    double sumTimeMs = 0.0;
//
//    // スロットインデックス
//    int vboSlot = 0;
//    int pboSlot = 0;
//
//    // 永続 PBO 用のマップ済みポインタ（任意）
//    unsigned char* pboMapped[NUM_PBO] = { nullptr, nullptr };
//    if (hasBufferStorage) {
//        for (int i = 0; i < NUM_PBO; ++i) {
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
//            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
//            pboMapped[i] = (unsigned char*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, colorBytes, flags);
//        }
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//    }
//
//    // CPU 変換の並列化用スレッド数
//    const unsigned workerCount = std::max(1u, std::thread::hardware_concurrency());
//
//    while (!glfwWindowShouldClose(win)) {
//
//        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(win, GL_TRUE);
//
//        auto t0 = std::chrono::high_resolution_clock::now();
//
//        // ---- RealSense フレーム取得 ----
//        rs2::frameset fs = pipe.wait_for_frames();
//        fs = align_to_color.process(fs);
//        rs2::depth_frame depth = fs.get_depth_frame();
//        rs2::video_frame color = fs.get_color_frame();
//
//        rs_points = pc.calculate(depth);
//        pc.map_to(color);
//        auto verts = rs_points.get_vertices();
//        auto tcoords = rs_points.get_texture_coordinates();
//        pointCount = rs_points.size();
//        if (pointCount == 0) continue;
//
//        // ---- CPU: Point 変換（並列） ----
//        std::vector<Point> staging(pointCount);
//        auto cpuWorker = [&](size_t b, size_t e) {
//            for (size_t i = b; i < e; ++i) {
//                const rs2::vertex& v = verts[i];
//                if (v.z <= 0.f) {
//                    staging[i] = { 1e9f,1e9f,1e9f, 0.f,0.f };
//                } else {
//                    const auto& uv = tcoords[i];
//                    staging[i] = { -v.x, -v.y, v.z + D, uv.u, uv.v };
//                }
//            }
//        };
//        std::vector<std::thread> workers;
//        workers.reserve(workerCount);
//        size_t chunk = (pointCount + workerCount - 1) / workerCount;
//        for (unsigned t = 0; t < workerCount; ++t) {
//            size_t b = size_t(t) * chunk;
//            size_t e = std::min(pointCount, b + chunk);
//            if (b < e) workers.emplace_back(cpuWorker, b, e);
//        }
//        for (auto& th : workers) th.join();
//
//        // ---- GPU: VBO 転送 ----
//        if (hasBufferStorage) {
//            // 再利用スロットのフェンスを確認
//            if (vboFences[vboSlot]) {
//                // 0 タイムアウトのポーリング（混雑時のみ待機）
//                GLenum st = glClientWaitSync(vboFences[vboSlot], 0, 0);
//                while (st == GL_TIMEOUT_EXPIRED) {
//                    // 必要な分だけ微待機（CPU 過負荷回避）
//                    std::this_thread::yield();
//                    st = glClientWaitSync(vboFences[vboSlot], 0, 0);
//                }
//                glDeleteSync(vboFences[vboSlot]);
//                vboFences[vboSlot] = 0;
//            }
//            // 書き込み
//            Point* dst = ringMapped + size_t(vboSlot) * slotPoints;
//            std::memcpy(dst, staging.data(), pointCount * sizeof(Point));
//
//            // VAO の頂点ポインタを該当スロットへ
//            glBindVertexArray(vao);
//            glBindBuffer(GL_ARRAY_BUFFER, vbo_ring);
//            const void* base = (void*)(size_t(vboSlot) * slotSize);
//            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), base);
//            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (const void*)(size_t(vboSlot) * slotSize + 3 * sizeof(float)));
//            glBindVertexArray(0);
//        } else {
//            // フォールバック: トリプル VBO をローテーション
//            GLuint cur = vbos[vboSlot];
//
//            // 前回使用済み VBO のフェンス確認（必要時のみ待機）
//            if (vboFences[vboSlot]) {
//                GLenum st = glClientWaitSync(vboFences[vboSlot], 0, 0);
//                while (st == GL_TIMEOUT_EXPIRED) {
//                    std::this_thread::yield();
//                    st = glClientWaitSync(vboFences[vboSlot], 0, 0);
//                }
//                glDeleteSync(vboFences[vboSlot]);
//                vboFences[vboSlot] = 0;
//            }
//
//            // オーファニングしてから glBufferSubData (DMA)
//            glBindBuffer(GL_ARRAY_BUFFER, cur);
//            glBufferData(GL_ARRAY_BUFFER, slotSize, nullptr, GL_STREAM_DRAW);
//            glBufferSubData(GL_ARRAY_BUFFER, 0, pointCount * sizeof(Point), staging.data());
//
//            // VAO に現在の VBO を結び直す（レイアウトは同じ）
//            glBindVertexArray(vao);
//            glBindBuffer(GL_ARRAY_BUFFER, cur);
//            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
//            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3 * sizeof(float)));
//            glBindVertexArray(0);
//        }
//
//        // ---- GPU: カラー転送（PBO 経由） ----
//        {
//            int next = pboSlot;
//            // PBO 再利用フェンス確認
//            if (pboFence[next]) {
//                GLenum st = glClientWaitSync(pboFence[next], 0, 0);
//                while (st == GL_TIMEOUT_EXPIRED) {
//                    std::this_thread::yield();
//                    st = glClientWaitSync(pboFence[next], 0, 0);
//                }
//                glDeleteSync(pboFence[next]);
//                pboFence[next] = 0;
//            }
//
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[next]);
//
//            if (hasBufferStorage) {
//                // 永続マップ済み領域へ memcpy
//                std::memcpy(pboMapped[next], color.get_data(), colorBytes);
//            } else {
//                // オーファニング + glBufferSubData
//                glBufferData(GL_PIXEL_UNPACK_BUFFER, colorBytes, nullptr, GL_STREAM_DRAW);
//                glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, colorBytes, color.get_data());
//            }
//
//            glBindTexture(GL_TEXTURE_2D, texColor);
//            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//            // PBO 経由で非同期にテクスチャへ（データポインタは nullptr）
//            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, PC_W, PC_H, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
//
//            glBindTexture(GL_TEXTURE_2D, 0);
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//        }
//
//        // ---- 描画 ----
//        glViewport(0, 0, WIN_W, WIN_H);
//        glClearColor(0.f, 0.f, 0.f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(prog);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texColor);
//        glUniform1i(locColor, 0);
//        glBindVertexArray(vao);
//
//        glEnable(GL_SCISSOR_TEST);
//        for (int r = 0; r < CAM_ROWS; ++r) {
//            for (int c = 0; c < CAM_COLS; ++c) {
//                int idx = r * CAM_COLS + c;
//                const Mat4& mvp = tileMVP[(size_t)idx];
//                glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp.m);
//
//                int vx = tileVX[(size_t)idx];
//                int vy = tileVY[(size_t)idx];
//
//                glViewport(vx, vy, VIEW_W, VIEW_H);
//                glScissor(vx, vy, VIEW_W, VIEW_H);
//                glClear(GL_DEPTH_BUFFER_BIT);
//                glDrawArrays(GL_POINTS, 0, (GLsizei)pointCount);
//            }
//        }
//        glDisable(GL_SCISSOR_TEST);
//        glBindVertexArray(0);
//        glUseProgram(0);
//
//        // フェンス: VBO/PBO 使用完了をマーク（GPU 側）
//        GLsync frameFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
//        // VBO スロットに紐付け
//        if (vboFences[vboSlot]) glDeleteSync(vboFences[vboSlot]);
//        vboFences[vboSlot] = frameFence;
//        // PBO スロットにも紐付け
//        if (pboFence[pboSlot]) glDeleteSync(pboFence[pboSlot]);
//        pboFence[pboSlot] = frameFence; // 同じフェンスで兼用（このフレーム内の全コマンド完了を示す）
//
//        // 次スロットへ
//        vboSlot = (vboSlot + 1) % NUM_SLOTS;
//        pboSlot = (pboSlot + 1) % NUM_PBO;
//
//        glfwSwapBuffers(win);
//        glfwPollEvents();
//
//        auto t1 = std::chrono::high_resolution_clock::now();
//        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
//        std::cout << ms << " ms" << std::endl;
//        sumTimeMs += ms;
//        frameNum++;
//    }
//
//    if (frameNum > 0) {
//        std::cout << "frame num:" << frameNum << ", Average time: " << (sumTimeMs / frameNum) << " ms" << std::endl;
//    }
//
//    // クリーンアップ
//    pipe.stop();
//    glDeleteProgram(prog);
//    glDeleteVertexArrays(1, &vao);
//
//    for (auto& f : vboFences) if (f) glDeleteSync(f);
//    for (int i = 0; i < NUM_PBO; ++i) if (pboFence[i]) glDeleteSync(pboFence[i]);
//
//    if (hasBufferStorage) {
//        glBindBuffer(GL_ARRAY_BUFFER, vbo_ring);
//        glUnmapBuffer(GL_ARRAY_BUFFER); // 永続マップ解除
//        glDeleteBuffers(1, &vbo_ring);
//        for (int i = 0; i < NUM_PBO; ++i) {
//            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
//            if (pboMapped[i]) glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
//        }
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//        glDeleteBuffers(NUM_PBO, pbo);
//    } else {
//        glDeleteBuffers(NUM_SLOTS, vbos);
//        glDeleteBuffers(NUM_PBO, pbo);
//    }
//
//    glDeleteTextures(1, &texColor);
//    glfwDestroyWindow(win);
//    glfwTerminate();
//    return 0;
//}