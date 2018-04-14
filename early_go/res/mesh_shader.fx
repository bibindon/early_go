float4x4 hlsl_world;
float4x4 hlsl_world_view_projection;
float4   hlsl_light_normal;
float    hlsl_light_brightness;
float4   hlsl_diffuse;
float4   hlsl_ambient = { 0.3f, 0.3f, 0.3f, 0.0f };

void vertex_shader(
    in  float4 in_position  : POSITION,
    in  float4 in_normal    : NORMAL0,
    in  float4 in_texture   : TEXCOORD0,

    out float4 out_position : POSITION,
    out float4 out_diffuse  : COLOR0,
    out float4 out_texture  : TEXCOORD0) {
    out_position  = mul(in_position, hlsl_world_view_projection);

    float light_intensity = hlsl_light_brightness
        * dot(in_normal, hlsl_light_normal);
    out_diffuse = hlsl_diffuse * max(0, light_intensity) + hlsl_ambient;

    out_texture  = in_texture;
}

texture hlsl_texture;
sampler texture_sampler = sampler_state {
    Texture   = (hlsl_texture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

void pixel_shader(
    in  float4 in_diffuse     : COLOR0,
    in  float2 in_texture     : TEXCOORD0,
    out float4 out_diffuse    : COLOR0) {
    out_diffuse = in_diffuse*0.8 * tex2D(texture_sampler, in_texture) + in_diffuse*0.2;
}

technique technique_ {
  pass pass_ {
    VertexShader = compile vs_2_0 vertex_shader();
    PixelShader  = compile ps_2_0 pixel_shader();
  }
}
