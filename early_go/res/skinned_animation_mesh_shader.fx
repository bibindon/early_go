float4 hlsl_light_normal;
float  hlsl_light_brightness;
float4 hlsl_light_diffuse = { 1.0f, 0.0f, 0.0f, 1.0f }; // TODO
float4 hlsl_diffuse;
float4 hlsl_ambient = { 0.3f, 0.3f, 0.3f, 0.0f };

/* Matrix Palette */
static const int MAX_MATRICES = 26;
float4x3         hlsl_world_matrix_array[MAX_MATRICES];
float4x4         hlsl_view_projection;

void vertex_shader(
  in  float4  in_position      : POSITION,
  in  float4  in_blend_weights : BLENDWEIGHT,
  in  float4  in_blend_indices : BLENDINDICES,
  in  float3  in_normal        : NORMAL,
  in  float3  in_texcoord0     : TEXCOORD0,
  out float4  out_position     : POSITION,
  out float4  out_diffuse      : COLOR,
  out float2  out_texcoord0    : TEXCOORD0,
  uniform int bone_number) {
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

    position += mul(in_position, hlsl_world_matrix_array[index_array[i]])
             * blend_weights_array[i];
    normal   += mul(float4(in_normal, 0.0f), hlsl_world_matrix_array[index_array[i]])
             * blend_weights_array[i];
  }

  last_weight = 1.0f - last_weight;

  // Now that we have the calculated weight, add in the final influence
  position += (mul(in_position,
      hlsl_world_matrix_array[index_array[bone_number - 1]]) * last_weight);
  normal   += (mul(float4(in_normal, 0.0f),
      hlsl_world_matrix_array[index_array[bone_number - 1]]) * last_weight);

  // transform position from world space into view and then projection space
  out_position = mul(float4(position.xyz, 1.0f), hlsl_view_projection);

  // normalize normals
  // normal = normalize(normal); // I dont now why.
  normal = normalize(-normal);


  float light_intensity = hlsl_light_brightness
      * dot(normal, hlsl_light_normal.xyz);
  // Shade (Ambient + etc.)
  out_diffuse.xyz = hlsl_diffuse.xyz * max(0, light_intensity) + hlsl_ambient.xyz;
  out_diffuse.w = 1.0f;

  // copy the input texture coordinate through
  out_texcoord0 = in_texcoord0.xy;
}

texture hlsl_texture;
sampler texture_sampler = sampler_state {
  Texture   = (hlsl_texture);

  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};

void pixel_shader(in  float4 in_diffuse  : COLOR0,
                  in  float2 in_texture  : TEXCOORD0,
                  out float4 out_diffuse : COLOR0) {
  out_diffuse = 0.8 * in_diffuse * tex2D(texture_sampler, in_texture) + 0.2 * in_diffuse;
}

int current_bone_numbers;
VertexShader vsArray[4] = { compile vs_2_0 vertex_shader(1),
                            compile vs_2_0 vertex_shader(2),
                            compile vs_2_0 vertex_shader(3),
                            compile vs_2_0 vertex_shader(4) };

technique technique_ {
  pass pass_ {
    VertexShader         = (vsArray[current_bone_numbers]);
    PixelShader          = compile ps_2_0 pixel_shader();
  }
}
