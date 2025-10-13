#version 330 core
in vec2 vUV;

out vec4 FragColor;

uniform sampler2D uAccum;

void main()
{
    vec4 acc = texture(uAccum, vUV);
    float w = max(acc.a, 1e-6);
    FragColor = vec4(acc.rgb / w, 1.0);
}