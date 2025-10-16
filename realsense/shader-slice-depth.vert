#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV; // 未使用だが頂点フォーマットを合わせるため残す

uniform float uLayerNdcZ; // このパス（層）のNDC深度 [-1,1]

void main(){
    gl_Position = vec4(aPos, uLayerNdcZ, 1.0);
}