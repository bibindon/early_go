float4x4 matWorld : world;            // ワールド変換行列
float4x4 matCameraView : view;        // カメラビュー変換行列
float4x4 matCameraProj : projection;  // 射影変換行列
float4x4 matLightView;                // ライトビュー変換行列
float4x4 matLightProj;                // 射影変換行列

texture texShadowMap;                 // シャドウマップテクスチャ
sampler DefSampler = sampler_state    // サンプラーステート
{
     texture = (texShadowMap);
     AddressU  = CLAMP;
     AddressV  = CLAMP;
     AddressW  = CLAMP;
     MIPFILTER = LINEAR;
     MINFILTER = LINEAR;
     MAGFILTER = LINEAR;
};

texture g_mesh_texture;
sampler mesh_texture_sampler = sampler_state {
    Texture   = (g_mesh_texture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture g_texture_0;

sampler texture_sampler[1] = {
    sampler_state {
        Texture = (g_texture_0);
    }
};

float2 g_position[8] = {
    {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
    {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
};
float  g_opacity[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };

struct VS_OUTPUT
{
    float4 Pos : POSITION;              // 射影変換座標
    float4 ZCalcTex : TEXCOORD1;        // Z値算出用テクスチャ
    float4 Col : COLOR0;                // ディフューズ色
    float4 texcood : TEXCOORD0;
};


// 頂点シェーダ
VS_OUTPUT DepthBufShadow_VS( float4 Pos : POSITION , float3 Norm : NORMAL, float4 in_texcood: TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // 普通にカメラの目線によるワールドビュー射影変換をする
    float4x4 mat;
    mat  = mul( matWorld, matCameraView );
    mat  = mul( mat, matCameraProj );
    Out.Pos = mul( Pos, mat );

    // ライトの目線によるワールドビュー射影変換をする
    mat  = mul( matWorld, matLightView );
    mat  = mul( mat, matLightProj );
    Out.ZCalcTex = mul( Pos, mat );

    // 法線とライトの方向から頂点の色を決定
    //  濃くなりすぎないように調節しています
    float3 N = normalize( mul(Norm, matWorld) );
    float3 LightDirect = normalize( float3(matLightView._13,matLightView._23,matLightView._33) );
    Out.Col = float4(1.0, 0.2, 0.2, 1.0) * (0.3 + dot(N, -LightDirect)*(1-0.3f));

    Out.texcood = in_texcood;

    return Out;
}

// ピクセルシェーダ
void DepthBufShadow_PS(
    in float4 Col : COLOR,
    in float4 ZCalcTex : TEXCOORD1,
    in float4 in_texcoord : TEXCOORD0,
    out float4 out_col : COLOR)
{
    float4 color_result = (float4)0;
    color_result = tex2D(mesh_texture_sampler, in_texcoord.xy);

    float4 color_tex[8];
    for (int i = 0; i <= 1; ++i)
    {
        color_tex[i] = tex2D(texture_sampler[0], in_texcoord.xy);
//        //color_tex[i].a *= g_opacity[i];
        color_result = (1.0f-color_tex[i].a) * color_result + color_tex[i].a * color_tex[i];
    }
//    out_col = (Col * color_result);
    out_col = color_result;

    // fade in/out
//    out_col.r *= (1.0f - g_opacity[7]);
//    out_col.g *= (1.0f - g_opacity[7]);
//    out_col.b *= (1.0f - g_opacity[7]);
//    out_col *= (1.0f - g_opacity[7]);

    out_col.a = 1.0f;

    // ライト目線によるZ値の再算出
    float ZValue = ZCalcTex.z / ZCalcTex.w;

    // テクスチャ座標に変換
    float2 TransTexCoord;
    TransTexCoord.x = (1.0f + ZCalcTex.x/ZCalcTex.w)*0.5f;
    TransTexCoord.y = (1.0f - ZCalcTex.y/ZCalcTex.w)*0.5f;

    // 同じ座標のZ値を抽出
    float SM_Z = tex2D( DefSampler, TransTexCoord ).y;

    // 算出点がシャドウマップのZ値よりも大きければ影と判断
    if ( ZValue > SM_Z+0.005f )
    {
        out_col.rgb = out_col.rgb * 0.5;
    }
    out_col.a = 1.0f;
}


// テクニック
technique DepthBufShadowTec
{
    pass P0
    {
        VertexShader = compile vs_2_0 DepthBufShadow_VS();
        PixelShader = compile ps_2_0 DepthBufShadow_PS();
    }
}
