#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2DArray uSlices;
uniform int   uNZ;
uniform float uInvZ[64];
uniform vec2  uBaseOffset;
uniform vec2  uFrustumUVShift;
uniform vec2  uUvScale;

// 追加: テクセルサイズ(= 1/W, 1/H)
uniform vec2  uTexelSize;

void main(){
    // タイル内UVスケール（中心0.5基準）
    vec2 uvBase = (vUV - vec2(0.5)) * uUvScale + vec2(0.5) + uFrustumUVShift;

    // 早期終了なし：必要に応じて維持/変更
    for(int n = uNZ - 1; n >= 0; --n){
        // レイヤnのパララックス
        vec2 uv = uvBase + uBaseOffset * uInvZ[n];

        // 半テクセル中心へシフト
        uv += 0.5 * uTexelSize;

        // 端で常に有効テクセル中心を取るよう内側にクランプ
        uv = clamp(uv, 0.5 * uTexelSize, vec2(1.0) - 0.5 * uTexelSize);

        vec4 c = texture(uSlices, vec3(uv, float(n)));
        if(c.a > 0.0){
            FragColor = c;
            return;
        }
    }
    discard;
}