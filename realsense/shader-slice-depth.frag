#version 330 core
out vec4 FragColor;

uniform sampler2DArray uSlices;       // RGBA8, A=存在フラグ
uniform sampler2D      uBaseOffsetTex; // RG32F, サイズ = (NUM_LENS_X, NUM_LENS_Y)

uniform int   uLayer;       // 現在の層インデックス
uniform int   uNZ;          // 総層数
uniform float uInvZ[64];    // 1/z テーブル（nに対応）
uniform float uInvZo;       // 1/MIN_OBSERVE_Z

uniform vec2  uUvScale;     // 既存と同じ
uniform vec2  uTexelSize;   // 1/W, 1/H（スライス解像度の逆数）
uniform ivec2 uTileSizePx;  // (NUM_ELEM_IMG_PX_X, NUM_ELEM_IMG_PX_Y)
uniform ivec2 uNumLens;     // (NUM_LENS_X, NUM_LENS_Y)

void main(){
    // 画面上のピクセル座標からタイルとタイル内UVを算出
    ivec2 pix = ivec2(gl_FragCoord.xy);
    ivec2 tile = pix / uTileSizePx;

    // グリッド外は破棄
    if(tile.x < 0 || tile.y < 0 || tile.x >= uNumLens.x || tile.y >= uNumLens.y){
        discard;
    }

    vec2 localUV = (vec2(pix - tile * uTileSizePx)) / vec2(uTileSizePx); // [0,1)

    // レンズオフセットをテクスチャから取得（行:下=0を想定）
    vec2 baseOffset = texelFetch(uBaseOffsetTex, tile, 0).rg;

    // 要素画像の基準UV（中心0.5基準）+ 視錐台シフト
    vec2 uvBase = (localUV - vec2(0.5)) * uUvScale + vec2(0.5) + baseOffset * uInvZo;

    // この層のパララックス
    vec2 uv = uvBase + baseOffset * uInvZ[uLayer];

    // 半テクセル中心補正 + 端クランプ
    uv += 0.5 * uTexelSize;
    uv = clamp(uv, 0.5 * uTexelSize, vec2(1.0) - 0.5 * uTexelSize);

    // サンプリング
    vec4 c = texture(uSlices, vec3(uv, float(uLayer)));

    // 非空だけ色・深度を書き込む（空=discardで深度は未更新）
    if(c.a <= 0.0){ discard; }
    FragColor = c;
}