//// コントローラの接続を待ち、接続後は押下/解放された入力をコンソールへ出力するサンプル。
//// 下位バージョン(GLFW 3.2以前)ヘッダでもコンパイルできるように、3.3以降で追加された
//// Gamepad API (glfwJoystickIsGamepad / glfwGetGamepadState / GLFWgamepadstate / glfwGetJoystickHats)
//// は存在判定付きで使用し、無い場合は「生ボタン/軸」のみを監視するフォールバックに切替える。
////
//// ビルド例:
////   g++ -std=c++17 controller_button_monitor.cpp -lglfw
//// (Windows / Visual Studio: 追加依存に glfw3.lib, 必要なら opengl32.lib 等)
////
//// 終了: ESC キー または Ctrl+C
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <vector>
//#include <thread>
//#include <chrono>
//#include <cmath>
//
//// 古いヘッダで GLFW_TRUE/GLFW_FALSE が未定義な場合の保険
//#ifndef GLFW_TRUE
//#define GLFW_TRUE 1
//#define GLFW_FALSE 0
//#endif
//
//// ---------------------------------------------
//// 機能存在判定 (ヘッダにシンボルが定義されているか)
//// ---------------------------------------------
//#if defined(GLFW_GAMEPAD_BUTTON_A)
//// GLFW 3.3 以降 (Gamepad API 一式が利用可能)
//#define HAS_GLFW_GAMEPAD_API 1
//#endif
//
//#if defined(GLFW_HAT_UP)
//// glfwGetJoystickHats / HAT 定数が利用可能 (3.3以降)
//#define HAS_GLFW_HAT_API 1
//#endif
//
//// 安全にランタイムバージョンを表示する補助
//static void printRuntimeGLFWVersion()
//{
//    int major = 0, minor = 0, rev = 0;
//    glfwGetVersion(&major, &minor, &rev);
//    std::cout << "[Runtime GLFW Version] " << major << "." << minor << "." << rev << "\n";
//    const char* verStr = glfwGetVersionString();
//    if (verStr) std::cout << "[Runtime Version String] " << verStr << "\n";
//}
//
//// Gamepad ボタン名 (マッピング API 利用時のみ)
//#if HAS_GLFW_GAMEPAD_API
//static const char* mappedButtonName(int i)
//{
//    switch (i)
//    {
//    case GLFW_GAMEPAD_BUTTON_A: return "A";
//    case GLFW_GAMEPAD_BUTTON_B: return "B";
//    case GLFW_GAMEPAD_BUTTON_X: return "X";
//    case GLFW_GAMEPAD_BUTTON_Y: return "Y";
//    case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return "LB";
//    case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return "RB";
//    case GLFW_GAMEPAD_BUTTON_BACK: return "Back";
//    case GLFW_GAMEPAD_BUTTON_START: return "Start";
//    case GLFW_GAMEPAD_BUTTON_GUIDE: return "Guide";
//    case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return "LThumb";
//    case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return "RThumb";
//    case GLFW_GAMEPAD_BUTTON_DPAD_UP: return "DPadUp";
//    case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return "DPadRight";
//    case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return "DPadDown";
//    case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return "DPadLeft";
//    default: return "?";
//    }
//}
//#endif
//
//int main()
//{
//    if (!glfwInit()) {
//        std::cerr << "GLFW 初期化失敗\n";
//        return 1;
//    }
//
//    printRuntimeGLFWVersion();
//
//    std::cout
//        << "コントローラ接続待機中...\n"
//        << "  (Gamepad API:"
//#ifdef HAS_GLFW_GAMEPAD_API
//        << "利用可能"
//#else
//        << "未利用(ヘッダが3.3未満)"
//#endif
//        << ", HAT:"
//#ifdef HAS_GLFW_HAT_API
//        << "利用可能"
//#else
//        << "未利用"
//#endif
//        << ")\n";
//
//    int joystickId = -1;
//
//    // 接続待ちループ (キャンセルしたければ Ctrl+C)
//    while (true) {
//        for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
//            if (glfwJoystickPresent(jid) == GLFW_TRUE) {
//                joystickId = jid;
//                break;
//            }
//        }
//        if (joystickId >= 0) break;
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//    }
//
//    const char* joyName = glfwGetJoystickName(joystickId);
//    std::cout << "ジョイスティック検出: id=" << joystickId
//        << " name='" << (joyName ? joyName : "unknown") << "'\n";
//
//#if HAS_GLFW_GAMEPAD_API
//    bool mapped = (glfwJoystickIsGamepad(joystickId) == GLFW_TRUE);
//    if (mapped) std::cout << "→ マッピング済みゲームパッドとして認識\n";
//    else        std::cout << "→ 未マッピング(生ジョイスティック)として認識\n";
//#else
//    bool mapped = false;
//    std::cout << "→ コンパイル時に Gamepad API が無いため生ジョイスティック扱い\n";
//#endif
//
//    std::cout << "ESCキー または Ctrl+C で終了\n\n";
//
//    // 非表示ウィンドウ (ESC処理 & pollEvents 用)
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//    GLFWwindow* window = glfwCreateWindow(160, 90, "ControllerHidden", nullptr, nullptr);
//    if (!window) {
//        std::cerr << "警告: ウィンドウ作成失敗。ESCによる終了検知は行えません。\n";
//    }
//
//    // 前回状態
//    std::vector<unsigned char> prevButtons;
//    std::vector<unsigned char> prevHats;
//    std::vector<float>         prevAxes;
//
//    while (true) {
//
//        if (window) {
//            glfwPollEvents();
//            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//                std::cout << "ESC検出 → 終了\n";
//                break;
//            }
//        }
//        else {
//            // window 無い場合でもジョイスティック状態は取得可能
//            // ただし OS によっては pollEvents が呼ばれないと更新が遅れる場合あり
//        }
//
//        // 切断検知
//        if (glfwJoystickPresent(joystickId) != GLFW_TRUE) {
//            std::cout << "ジョイスティック切断。再スキャン...\n";
//            joystickId = -1;
//#if HAS_GLFW_GAMEPAD_API
//            mapped = false;
//#endif
//            prevButtons.clear();
//            prevHats.clear();
//            prevAxes.clear();
//
//            while (true) {
//                for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
//                    if (glfwJoystickPresent(jid) == GLFW_TRUE) { joystickId = jid; break; }
//                }
//                if (joystickId >= 0) {
//                    joyName = glfwGetJoystickName(joystickId);
//#if HAS_GLFW_GAMEPAD_API
//                    mapped = (glfwJoystickIsGamepad(joystickId) == GLFW_TRUE);
//#endif
//                    std::cout << "再接続: id=" << joystickId
//                        << " name='" << (joyName ? joyName : "unknown") << "'"
//#if HAS_GLFW_GAMEPAD_API
//                        << (mapped ? " [mapped]" : " [raw]")
//#endif
//                        << "\n";
//                    break;
//                }
//                std::this_thread::sleep_for(std::chrono::milliseconds(500));
//            }
//            continue;
//        }
//
//#if HAS_GLFW_GAMEPAD_API
//        if (mapped) {
//            GLFWgamepadstate state{};
//            if (glfwGetGamepadState(joystickId, &state)) {
//
//                if (prevButtons.empty())
//                    prevButtons.assign(state.buttons, state.buttons + 15);
//                if (prevAxes.empty())
//                    prevAxes.assign(state.axes, state.axes + 6);
//
//                // ボタン変化
//                for (int i = 0; i < 15; ++i) {
//                    unsigned char cur = state.buttons[i];
//                    unsigned char prev = prevButtons[i];
//                    if (cur != prev) {
//                        std::cout << "[BTN] " << mappedButtonName(i)
//                            << (cur ? " Pressed" : " Released") << "\n";
//                        prevButtons[i] = cur;
//                    }
//                }
//                // 軸変化（閾値）
//                for (int i = 0; i < 6; ++i) {
//                    float cur = state.axes[i];
//                    float prev = prevAxes[i];
//                    if (std::fabs(cur - prev) > 0.15f) {
//                        std::cout << "[AXIS] " << i << " = " << cur << "\n";
//                        prevAxes[i] = cur;
//                    }
//                }
//            }
//        }
//        else
//#endif
//        {
//            // 生ジョイスティックフォールバック
//            int btnCount = 0;
//            const unsigned char* btns = glfwGetJoystickButtons(joystickId, &btnCount);
//
//            if (prevButtons.size() != (size_t)btnCount)
//                prevButtons.assign(btns, btns + btnCount);
//
//            for (int i = 0; i < btnCount; ++i) {
//                unsigned char cur = btns[i];
//                unsigned char prev = prevButtons[i];
//                if (cur != prev) {
//                    std::cout << "[RAW BTN] #" << i << (cur ? " Pressed" : " Released") << "\n";
//                    prevButtons[i] = cur;
//                }
//            }
//
//#if HAS_GLFW_HAT_API
//            int hatCount = 0;
//            const unsigned char* hats = glfwGetJoystickHats(joystickId, &hatCount);
//            if (prevHats.size() != (size_t)hatCount)
//                prevHats.assign(hats, hats + hatCount);
//            for (int i = 0; i < hatCount; ++i) {
//                unsigned char cur = hats[i];
//                unsigned char prev = prevHats[i];
//                if (cur != prev) {
//                    std::cout << "[HAT] #" << i << " value=" << (int)cur << "\n";
//                    prevHats[i] = cur;
//                }
//            }
//#endif
//
//            int axisCount = 0;
//            const float* axes = glfwGetJoystickAxes(joystickId, &axisCount);
//            if (prevAxes.size() != (size_t)axisCount)
//                prevAxes.assign(axes, axes + axisCount);
//            for (int i = 0; i < axisCount; ++i) {
//                float cur = axes[i];
//                float prev = prevAxes[i];
//                if (std::fabs(cur - prev) > 0.15f) {
//                    std::cout << "[RAW AXIS] #" << i << " = " << cur << "\n";
//                    prevAxes[i] = cur;
//                }
//            }
//        }
//
//        // CPU 負荷軽減
//        std::this_thread::sleep_for(std::chrono::milliseconds(16));
//    }
//
//    if (window) glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}