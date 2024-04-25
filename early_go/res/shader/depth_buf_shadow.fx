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
texture g_texture_1;
texture g_texture_2;
texture g_texture_3;
texture g_texture_4;
texture g_texture_5;
texture g_texture_6;
texture g_texture_7;

sampler texture_sampler[8] = {
    sampler_state
    {
        Texture = (g_texture_0);
    },
    sampler_state
    {
        Texture = (g_texture_1);
    },
    sampler_state
    {
        Texture = (g_texture_2);
    },
    sampler_state
    {
        Texture = (g_texture_3);
    },
    sampler_state
    {
        Texture = (g_texture_4);
    },
    sampler_state
    {
        Texture = (g_texture_5);
    },
    sampler_state
    {
        Texture = (g_texture_6);
    },
    sampler_state
    {
        Texture = (g_texture_7);
    },
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
VS_OUTPUT DepthBufShadow_VS(
    float4 Pos : POSITION,
    float3 Norm : NORMAL,
    float4 in_texcood: TEXCOORD0)
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
    Out.Col = float4(3.0, 3.0, 3.0, 3.0) * (0.5 + dot(N, -LightDirect)*(1-0.5f));

    Out.texcood = in_texcood;

    return Out;
}
static const int MAX_MATRICES = 26;
float4x3 g_world_matrix_array[MAX_MATRICES];
// 頂点シェーダ
void DepthBufShadow_VS2(
    in  float4  in_position      : POSITION,
    in  float4  in_blend_weights : BLENDWEIGHT,
    in  float4  in_blend_indices : BLENDINDICES,
    in  float4  in_normal        : NORMAL,
    in  float3  in_texcoord0     : TEXCOORD0,

    out float4  out_position     : POSITION,
    out float4  out_diffuse      : COLOR,
    out float2  out_texcoord0    : TEXCOORD0,
    uniform int bone_number)
{
    float3 position    = 0.0f;
    float3 normal      = 0.0f;
    float  last_weight = 0.0f;

    // Compensate for lack of UBYTE4 on Geforce3
    int4 index_vector = D3DCOLORtoUBYTE4(in_blend_indices);

    // cast the vectors to arrays for use in the for loop below
    float blend_weights_array[4] = (float[4])in_blend_weights;
    int   index_array[4]         = (int[4])index_vector;

    // calculate the pos/normal using the "normal" weights
    //        and accumulate the weights to calculate the last weight
    [unroll] for (int i = 0; i < bone_number - 1; ++i)
    {
        last_weight = last_weight + blend_weights_array[i];

        position += mul(in_position, g_world_matrix_array[index_array[i]]) * blend_weights_array[i];
        normal += mul(in_normal, g_world_matrix_array[index_array[i]]) * blend_weights_array[i];
    }

    last_weight = 1.0f - last_weight;

    // Now that we have the calculated weight, add in the final influence
    position += (mul(in_position, g_world_matrix_array[index_array[bone_number - 1]]) * last_weight);
    normal += (mul(in_normal, g_world_matrix_array[index_array[bone_number - 1]]) * last_weight);

    // transform position from world space into view and then projection space
    float4x4 mat;
    mat = mul (matCameraView, matCameraProj);
    out_position = mul(float4(position.xyz, 1.0f), mat);

    normal -= position; // Parallel light source
    float4 normal4 = normalize(float4(normal.xyz, 1.0f));


//    float light_intensity = 1.0f * dot(normal4, g_light_normal);
    // Shade (Ambient + etc.)
//  out_diffuse = g_light_diffuse * g_diffuse * max(0, light_intensity) + g_ambient;
//    out_diffuse = g_diffuse * max(0, light_intensity) + g_ambient;
//    out_diffuse.a = 1.0f;
    float3 N = normalize( mul(in_normal, matWorld) );
    float3 LightDirect = normalize( float3(matLightView._13,matLightView._23,matLightView._33) );
    out_diffuse = float4(1.0, 1.0, 1.0, 1.0) * (0.5 + dot(normal4, -LightDirect)*(1-0.4f));
    out_diffuse.rgb += 0.2f;

    // copy the input texture coordinate through
    out_texcoord0 = in_texcoord0.xy;
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
    for (int i = 0; i < 5; ++i)
    {
        color_tex[i] = tex2D(texture_sampler[i], in_texcoord.xy + g_position[i]);
//        //color_tex[i].a *= g_opacity[i];
        color_result = (1.0f-color_tex[i].a) * color_result + color_tex[i].a * color_tex[i];
    }
    out_col = (Col * color_result);
//    out_col = color_result;

    // fade in/out
    out_col *= (1.0f - g_opacity[7]);
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

int current_bone_numbers;
VertexShader vsArray[4] = {
    compile vs_2_0 DepthBufShadow_VS2(1),
    compile vs_2_0 DepthBufShadow_VS2(2),
    compile vs_2_0 DepthBufShadow_VS2(3),
    compile vs_2_0 DepthBufShadow_VS2(4)
};

// テクニック
technique DepthBufShadowTec
{
    pass P0
    {
        VertexShader = compile vs_2_0 DepthBufShadow_VS();
        PixelShader = compile ps_2_0 DepthBufShadow_PS();
    }
    pass P1
    {
        VertexShader = (vsArray[current_bone_numbers]);
        PixelShader = compile ps_2_0 DepthBufShadow_PS();
    }
}
