float4x4 hlsl_world_view_projection;
float4   hlsl_light_position;
float    hlsl_light_brightness;
float    hlsl_model_scale;
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

    float4 _light_position_vector = hlsl_light_position - in_position;
    float  _light_vector_length =
        length(_light_position_vector) * hlsl_model_scale;
    float4 _light_position_normal_vector = normalize(_light_position_vector);
    float4 _normal_vector = normalize(in_normal);
    float  _light_intensity = hlsl_light_brightness
        * dot(_normal_vector, _light_position_normal_vector)
        / (_light_vector_length * _light_vector_length);
    out_diffuse = hlsl_diffuse * max(0, _light_intensity) + hlsl_ambient;

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
    out_diffuse = in_diffuse * tex2D(texture_sampler, in_texture);
}

technique technique_ {
  pass pass_ {
    ZEnable              = TRUE;
    MultiSampleAntialias = TRUE;
    CullMode             = CCW;
    ShadeMode            = GOURAUD;

    VertexShader = compile vs_2_0 vertex_shader();
    PixelShader  = compile ps_2_0 pixel_shader();
  }
}
