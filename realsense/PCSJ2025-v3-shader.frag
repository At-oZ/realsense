#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2DArray uSlices;        // Zレイヤ = NUM_Z_PLANE
uniform sampler2D      uBaseOffsetTex; // (NUM_LENS_X x NUM_LENS_Y), RG32F
uniform int   uNZ;
uniform vec2  uUvScale;
uniform vec2  uTexelSize;
uniform float uInvZ[60];               // 必要に応じてサイズを調整
uniform vec2  uScreenSizePx;           // (NUM_DISPLAY_IMG_PX_X, NUM_DISPLAY_IMG_PX_Y)
uniform vec2  uTileSizePx;             // (NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y)
uniform vec2  uNumLens;                // (NUM_LENS_X, NUM_LENS_Y)
uniform float uInvZo;                  // 1.0 / MIN_OBSERVE_Z

void main()
{
    vec2 frag = gl_FragCoord.xy;

    // ビューポート = 画面領域だけ描画
    if (any(lessThan(frag, vec2(0.0))) || any(greaterThanEqual(frag, uScreenSizePx))) {
        discard;
    }

    // タイルIDとタイル内UV(0..1)
    ivec2 tileXY = ivec2(floor(frag / uTileSizePx));
    if (any(lessThan(tileXY, ivec2(0))) || any(greaterThanEqual(tileXY, ivec2(uNumLens)))) {
        discard;
    }
    vec2 localUV = (frag - vec2(tileXY) * uTileSizePx) / uTileSizePx; // 0..1

    // タイルごとの baseOffset（テクスチャUV単位）
    vec2 baseOffset = texelFetch(uBaseOffsetTex, tileXY, 0).rg;

    // 重要: 中心(0.5,0.5)基準でスケーリングしてから中心に戻す
    // 旧実装（タイル毎のビューポート）と同じ座標基準に合わせる
    vec2 uv = (localUV - vec2(0.5)) * uUvScale + vec2(0.5);

    vec3 acc = vec3(0.0);
    float w = 0.0;

    // 各層合成（A==0はスキップ）
    for (int nz = 0; nz < uNZ; ++nz) {
        float invz = uInvZ[nz];

        // 旧実装: uBaseOffset*invz + uFrustumUVShift(uBaseOffset * uInvZo)
        vec2 parallax = baseOffset * (invz + uInvZo);

        vec2 sampleUV = uv + parallax;
        // ラップは CLAMP_TO_EDGE 指定済みだが、極端なズレを避けたければ clamp してもよい
        // sampleUV = clamp(sampleUV, vec2(0.0), vec2(1.0));

        vec4 rgba = texture(uSlices, vec3(sampleUV, float(nz)));
        if (rgba.a > 0.5) { // Aは存在フラグ（CPU側で numPoints>0 → 255）
            acc += rgba.rgb;
            w += 1.0;
        }
    }

    FragColor = (w > 0.0) ? vec4(acc / w, 1.0) : vec4(0.0);
}