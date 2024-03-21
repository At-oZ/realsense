//#include <glad/gl.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//const unsigned int N = 256; // 画像の幅と高さ
//const unsigned int SCREEN_WIDTH = 800; // ウィンドウの幅
//const unsigned int SCREEN_HEIGHT = 600; // ウィンドウの高さ
//
//// 画像データを生成します。(例として、赤、緑、青のグラデーションを生成)
//void generateImageData(unsigned char* data) {
//    for (int y = 0; y < N; ++y) {
//        for (int x = 0; x < N; ++x) {
//            int index = (y * N + x) * 3;
//            data[index] = (unsigned char)(x % 256); // 赤
//            data[index + 1] = (unsigned char)(y % 256); // 緑
//            data[index + 2] = (unsigned char)((x + y) % 256); // 青
//        }
//    }
//}
//
//int main() {
//
//    const char* vertexShaderSource = "#version 330 core\n"
//        "layout (location = 0) in vec3 aPos;\n"
//        "layout (location = 1) in vec2 aTexCoord;\n"
//        "out vec2 TexCoord;\n"
//        "void main()\n"
//        "{\n"
//        "   gl_Position = vec4(aPos, 1.0);\n"
//        "   TexCoord = aTexCoord;\n"
//        "}\0";
//
//    const char* fragmentShaderSource = "#version 330 core\n"
//        "out vec4 FragColor;\n"
//        "in vec2 TexCoord;\n"
//        "uniform sampler2D ourTexture;\n"
//        "void main()\n"
//        "{\n"
//        "   FragColor = texture(ourTexture, TexCoord);\n"
//        "}\0";
//
//    // 頂点シェーダーのコンパイル
//    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//    glCompileShader(vertexShader);
//
//    // フラグメントシェーダーのコンパイル
//    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//    glCompileShader(fragmentShader);
//
//    // シェーダープログラムのリンク
//    unsigned int shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//
//    // 使用していないシェーダーの削除
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    float vertices[] = {
//        // 位置           // テクスチャ座標
//        0.5f,  0.5f, 0.0f,  1.0f, 1.0f,   // 右上
//        0.5f, -0.5f, 0.0f,  1.0f, 0.0f,   // 右下
//        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // 左下
//        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f   // 左上
//    };
//
//    unsigned int indices[] = {  // 注意：0から始まります!
//        0, 1, 3,   // 第一の三角形
//        1, 2, 3    // 第二の三角形
//    };
//
//    unsigned int VBO, VAO, EBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//
//    // バッファと頂点属性を設定
//    glBindVertexArray(VAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    // 位置属性
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    // テクスチャ属性
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    // GLFWの初期化
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // ウィンドウの作成
//    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "N x N Image", nullptr, nullptr);
//    if (window == nullptr) {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    // GLADの初期化
//    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // ビューポートの設定
//    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//
//    // 画像データを格納するための配列
//    unsigned char imageData[N * N * 3];
//    generateImageData(imageData);
//
//    // テクスチャを生成して設定
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, N, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
//
//    // 描画ループ
//    while (!glfwWindowShouldClose(window)) {
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // テクスチャをバインド
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        // シェーダープログラムを使用
//        glUseProgram(shaderProgram);
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//        // 他のイベントをチェックし、バッファを交換
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    //while (!glfwWindowShouldClose(window)) {
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    glBindTexture(GL_TEXTURE_2D, texture);
//
//    //    // ここで画像をウィンドウに描画するためのコードを追加します
//    //    // 注: この部分には、適切なシェーダーと描画コマンドを使用するコードが必要です。
//
//    //    glfwSwapBuffers(window);
//    //    glfwPollEvents();
//    //}
//
//    glfwTerminate();
//    return 0;
//}
