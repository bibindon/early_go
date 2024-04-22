float4x4 matWorld : world;        // ワールド変換行列
float4x4 matView : view;          // ビュー変換行列
float4x4 matProj : projection;    // 射影変換行列

struct VS_OUTPUT
{
   float4 Pos : POSITION;   // 射影変換座標
   float4 ShadowMapTex : TEXCOORD0;   // Zバッファテクスチャ
};

// 頂点シェーダ
VS_OUTPUT ZBufferCalc_VS( float4 Pos : POSITION )
{
   VS_OUTPUT Out = (VS_OUTPUT)0;

   // 普通にワールドビュー射影行列をする
   float4x4 mat;
   mat = mul( matWorld, matView );
   mat = mul( mat, matProj );
   Out.Pos = mul( Pos, mat );
   
   // テクスチャ座標を頂点に合わせる
   Out.ShadowMapTex = Out.Pos;

   return Out;
}


// Matrix Palette 
static const int MAX_MATRICES = 26;
float4x3 g_world_matrix_array[MAX_MATRICES];
//float4x4 g_view_projection;

VS_OUTPUT ZBufferCalc_VS2(
    in  float4  Pos : POSITION,
    in  float4  in_blend_weights : BLENDWEIGHT,
    in  float4  in_blend_indices : BLENDINDICES,
    in  float4  in_normal        : NORMAL,
    in  float3  in_texcoord0     : TEXCOORD0,
    uniform int bone_number
)
{
   VS_OUTPUT Out = (VS_OUTPUT)0;

   // 普通にワールドビュー射影行列をする
//   float4x4 mat;
//   mat = mul( matWorld, matView );
//   mat = mul( mat, matProj );
//   Out.Pos = mul( Pos, mat );

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

        position += mul(Pos, g_world_matrix_array[index_array[i]]) * blend_weights_array[i];
        normal += mul(in_normal, g_world_matrix_array[index_array[i]]) * blend_weights_array[i];
    }

    last_weight = 1.0f - last_weight;

    // Now that we have the calculated weight, add in the final influence
    position = (mul(Pos, g_world_matrix_array[index_array[0]]));
    normal += (mul(in_normal, g_world_matrix_array[index_array[bone_number - 1]]) * last_weight);

    // transform position from world space into view and then projection space
    float4x4 mat;
    mat = mul (matView, matProj);
    Out.Pos = mul(float4(position.xyz, 1.0f), mat);

    // テクスチャ座標を頂点に合わせる
    Out.ShadowMapTex = Out.Pos;

   return Out;
}

// ピクセルシェーダ
float4 ZBufferPlot_PS(float4 ShadowMapTex : TEXCOORD0) : COLOR
{
    // Z値算出
    float4 work;
    work.a = 1.0f;
    work.r = ShadowMapTex.z / ShadowMapTex.w;
    work.g = ShadowMapTex.z / ShadowMapTex.w;
    work.b = ShadowMapTex.z / ShadowMapTex.w;
    return work;
}

int current_bone_numbers;
VertexShader vsArray[4] = {
    compile vs_2_0 ZBufferCalc_VS2(1),
    compile vs_2_0 ZBufferCalc_VS2(2),
    compile vs_2_0 ZBufferCalc_VS2(3),
    compile vs_2_0 ZBufferCalc_VS2(4)
};

technique ZValuePlotTec
{
    pass P0
    {
        VertexShader = compile vs_2_0 ZBufferCalc_VS();
        PixelShader = compile ps_2_0 ZBufferPlot_PS();
    }
    pass P1
    {
        VertexShader = (vsArray[current_bone_numbers]);
        PixelShader = compile ps_2_0 ZBufferPlot_PS();
    }
}
