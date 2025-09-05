//// OpenGLの基本的なセットアップとウィンドウの作成(正しくインストールできているかチェック用)
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//int main() {
//    // GLFWの初期化
//    if (!glfwInit()) {
//        std::cerr << "GLFW initialization failed" << std::endl;
//        return -1;
//    }
//
//    // OpenGLのバージョン指定
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // ウィンドウの作成
//    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
//    if (!window) {
//        std::cerr << "Window creation failed" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    // OpenGLコンテキストの作成
//    glfwMakeContextCurrent(window);
//
//    // GLAD の初期化
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cerr << "GLAD initialization failed" << std::endl;
//        return -1;
//    }
//
//    // メインループ
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_COLOR_BUFFER_BIT);
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}