#version 330 core
in vec3 outColor;
out vec4 FragColor;

uniform float uWeight; // 重み（とりあえず1.0）

void main()
{
    float w = uWeight;
    FragColor = vec4(outColor * w, w); // rgb=色の総和, a=重み総和
}