float4x4 g_world;
float4x4 g_world_view_projection;
float4 g_light_normal;
float g_light_brightness;
float4 g_diffuse;
float4 g_ambient = { 0.2f, 0.2f, 0.2f, 0.0f };

float4x4 matLightView;                // ライトビュー変換行列
float4x4 matLightProj;                // 射影変換行列

void vertex_shader(
    in  float4 in_position  : POSITION,
    in  float4 in_normal    : NORMAL0,
    in  float4 in_texcood   : TEXCOORD0,

    out float4 out_position : POSITION,
    out float4 out_diffuse  : COLOR0,
    out float4 out_texcood  : TEXCOORD0)
{
    out_position  = mul(in_position, g_world_view_projection);

    float light_intensity = g_light_brightness * dot(in_normal, g_light_normal);
    out_diffuse = g_diffuse * max(0, light_intensity) + g_ambient;
    out_diffuse.a = 1.0f;

    //out_texcood = in_texcood;

    
    // ライトの目線によるワールドビュー射影変換をする
    float4x4 mat;
    mat  = mul( g_world, matLightView );
    mat  = mul( mat, matLightProj );
    out_texcood = mul( in_position, mat );
}

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
    sampler_state { Texture = (g_texture_0); },
    sampler_state { Texture = (g_texture_1); },
    sampler_state { Texture = (g_texture_2); },
    sampler_state { Texture = (g_texture_3); },
    sampler_state { Texture = (g_texture_4); },
    sampler_state { Texture = (g_texture_5); },
    sampler_state { Texture = (g_texture_6); },
    sampler_state { Texture = (g_texture_7); }
};

float2 g_position[8] = {
    {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
    {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
};
float  g_opacity[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };

void pixel_shader(
    in  float4 in_diffuse  : COLOR0,
    //in  float2 in_texcood  : TEXCOORD0,
    in  float4 ZCalcTex  : TEXCOORD0,
    out float4 out_diffuse : COLOR0)
{
    // ライト目線によるZ値の再算出
    float ZValue = ZCalcTex.z / ZCalcTex.w;

    // テクスチャ座標に変換
    float2 TransTexCoord;
    TransTexCoord.x = (1.0f + ZCalcTex.x/ZCalcTex.w)*0.5f;
    TransTexCoord.y = (1.0f - ZCalcTex.y/ZCalcTex.w)*0.5f;

    // 同じ座標のZ値を抽出
    float SM_Z = tex2D( texture_sampler[0], TransTexCoord ).w;

    // 算出点がシャドウマップのZ値よりも大きければ影と判断
    if ( ZValue > SM_Z+0.005f )
    {
//        Col.rgb = Col.rgb * 0.5;
        in_diffuse.rgb = in_diffuse.rgb * (0.5*ZValue * ZValue * ZValue ); // use pow
    }

    out_diffuse = in_diffuse;
    /*
    float4 color_result = (float4)0;

    color_result = tex2D(mesh_texture_sampler, in_texcood);

    float4 color_tex[8];
    for (int i = 0; i < (8-1); ++i)
    {
        color_tex[i] = tex2D(texture_sampler[i], in_texcood + g_position[i]);
        color_tex[i].a *= g_opacity[i];
        color_result = (1.0f-color_tex[i].a) * color_result + color_tex[i].a * color_tex[i];
    }
    out_diffuse = (in_diffuse * color_result);
    out_diffuse.a = 1.0f;

    // fade in/out
    out_diffuse.r *= (1.0f - g_opacity[7]);
    out_diffuse.g *= (1.0f - g_opacity[7]);
    out_diffuse.b *= (1.0f - g_opacity[7]);
    */
}

technique technique_
{
    pass pass_
    {
        VertexShader = compile vs_2_0 vertex_shader();
        PixelShader  = compile ps_2_0 pixel_shader();
    }
}
