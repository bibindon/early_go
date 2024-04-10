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

technique ZValuePlotTec
{
   pass P0
   {
      VertexShader = compile vs_2_0 ZBufferCalc_VS();
      PixelShader = compile ps_2_0 ZBufferPlot_PS();
   }
}
