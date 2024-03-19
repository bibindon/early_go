float4 g_light_normal;
float g_light_brightness;
//float4 g_light_diffuse = {1, 0, 0, 0}; // TODO
float4 g_diffuse;
float4 g_ambient = { 0.2f, 0.2f, 0.5f, 0.0f };

// Matrix Palette 
static const int MAX_MATRICES = 26;
float4x3 g_world_matrix_array[MAX_MATRICES];
float4x4 g_view_projection;

void vertex_shader(
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
    out_position = mul(float4(position.xyz, 1.0f), g_view_projection);

    normal -= position; // Parallel light source
    float4 normal4 = normalize(float4(normal.xyz, 1.0f));


    float light_intensity = g_light_brightness * dot(normal4, g_light_normal);
    // Shade (Ambient + etc.)
//  out_diffuse = g_light_diffuse * g_diffuse * max(0, light_intensity) + g_ambient;
    out_diffuse = g_diffuse * max(0, light_intensity) + g_ambient;
    out_diffuse.a = 1.0f;

    // copy the input texture coordinate through
    out_texcoord0 = in_texcoord0.xy;
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
    in  float2 in_texcood  : TEXCOORD0,
    out float4 out_diffuse : COLOR0)
{
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
}

int current_bone_numbers;
VertexShader vsArray[4] = {
    compile vs_2_0 vertex_shader(1),
    compile vs_2_0 vertex_shader(2),
    compile vs_2_0 vertex_shader(3),
    compile vs_2_0 vertex_shader(4)
};

technique technique_
{
    pass pass_
    {
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;

        VertexShader = (vsArray[current_bone_numbers]);
        PixelShader = compile ps_2_0 pixel_shader();
    }
}
