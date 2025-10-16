#version 330 core
layout(location=0) in vec2 aPos; // [-1,1] full screen quad
layout(location=1) in vec2 aUV;

out vec2 vUV; // ñ¢égópÇ≈Ç‡ï€éù

void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}