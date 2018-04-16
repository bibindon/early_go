float4x4 g_world;
float4x4 g_world_view_projection;
float4   g_light_normal;
float    g_light_brightness;
float4   g_diffuse;
float4   g_ambient = { 0.3f, 0.3f, 0.3f, 0.0f };

void vertex_shader(
    in  float4 in_position  : POSITION,
    in  float4 in_normal    : NORMAL0,
    in  float4 in_texcood   : TEXCOORD0,

    out float4 out_position : POSITION,
    out float4 out_diffuse  : COLOR0,
    out float4 out_texture  : TEXCOORD0) {
    out_position  = mul(in_position, g_world_view_projection);
    in_normal = mul(in_normal, g_world);
    in_normal = normalize(in_normal);

    float light_intensity = g_light_brightness
        * dot(in_normal, g_light_normal);
    out_diffuse = g_diffuse * max(0, light_intensity) + g_ambient;
    out_diffuse.a = 1.0f;

    out_texture  = in_texcood;
}

texture g_texture;
sampler texture_sampler = sampler_state {
    Texture   = (g_texture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

void pixel_shader(
    in  float4 in_diffuse     : COLOR0,
    in  float2 in_texcood     : TEXCOORD0,
    out float4 out_diffuse    : COLOR0) {
    out_diffuse = in_diffuse*0.8 * tex2D(texture_sampler, in_texcood) + in_diffuse*0.2;
}

technique technique_ {
  pass pass_ {
    VertexShader = compile vs_2_0 vertex_shader();
    PixelShader  = compile ps_2_0 pixel_shader();
  }
}
