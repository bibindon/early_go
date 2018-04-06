float4 hlsl_light_position;
float  hlsl_light_brightness;
float4 hlsl_light_diffuse = { 1.0f, 0.0f, 0.0f, 1.0f }; // TODO
float  hlsl_model_scale; // TODO
float4 hlsl_diffuse;
float4 hlsl_ambient = { 0.3f, 0.3f, 0.3f, 0.0f };

/* Matrix Palette */
static const int MAX_MATRICES = 26;
float4x3         hlsl_world_matrix_array[MAX_MATRICES];
float4x4         hlsl_projection;

struct VS_INPUT
{
  float4  position      : POSITION;
  float4  blend_weights : BLENDWEIGHT;
  float4  blend_indices : BLENDINDICES;
  float4  normal        : NORMAL;
  float3  texcoord0     : TEXCOORD0;
};

struct VS_OUTPUT
{
  float4  position  : POSITION;
  float4  diffuse   : COLOR;
  float2  texcoord0 : TEXCOORD0;
};

float4 diffuse(float4 normal, float4 position)
{
  float cosine_theta;

  // TODO verify
  //float4 light_vector = hlsl_light_position - position;
  float4 light_vector = position - hlsl_light_position;

  cosine_theta = max(0.0f, hlsl_light_brightness * dot(normal, light_vector));

  // propogate scalar result to vector
  return (cosine_theta);
}

VS_OUTPUT vertex_shader(
    VS_INPUT input, VS_OUTPUT output, uniform int bone_number)
{
  float3 position = 0.0f;
  float3 normal = 0.0f;
  float  last_weight = 0.0f;

  // Compensate for lack of UBYTE4 on Geforce3
  int4 index_vector = D3DCOLORtoUBYTE4(input.blend_indices);

  // cast the vectors to arrays for use in the for loop below
  float blend_weights_array[4] = (float[4])input.blend_weights;
  int   index_array[4] = (int[4])index_vector;

  // calculate the pos/normal using the "normal" weights
  //        and accumulate the weights to calculate the last weight
  [unroll] for (int i = 0; i < bone_number - 1; ++i)
  {
    last_weight = last_weight + blend_weights_array[i];

    position += mul(input.position, hlsl_world_matrix_array[index_array[i]])
        * blend_weights_array[i];
    normal += mul(input.normal, hlsl_world_matrix_array[index_array[i]])
        * blend_weights_array[i];
  }

  last_weight = 1.0f - last_weight;

  // Now that we have the calculated weight, add in the final influence
  position += (mul(input.position,
      hlsl_world_matrix_array[index_array[bone_number - 1]]) * last_weight);
  normal += (mul(input.normal,
      hlsl_world_matrix_array[index_array[bone_number - 1]]) * last_weight);

  // transform position from world space into view and then projection space
  output.position = mul(float4(position.xyz, 1.0f), hlsl_projection);

  // normalize normals
  normal = normalize(normal);

  // Shade (Ambient + etc.)
  output.diffuse = hlsl_ambient
      + diffuse(input.normal, input.position) * hlsl_diffuse;
  output.diffuse.w = 0.5f;

  // copy the input texture coordinate through
  output.texcoord0 = input.texcoord0.xy;

  return output;
}

texture hlsl_texture;
sampler texture_sampler = sampler_state {
  Texture = (hlsl_texture);

  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};

void pixel_shader(in  float4 in_diffuse  : COLOR0,
                  in  float2 in_texture : TEXCOORD0,
                  out float4 out_diffuse : COLOR0)
{
  out_diffuse = in_diffuse * tex2D(texture_sampler, in_texture);
}

int current_bone_numbers;
VertexShader vsArray[4] = { compile vs_2_0 vertex_shader(1),
                            compile vs_2_0 vertex_shader(2),
                            compile vs_2_0 vertex_shader(3),
                            compile vs_2_0 vertex_shader(4) };

technique technique_ {
  pass pass_ {
    VertexShader = (vsArray[current_bone_numbers]);
    PixelShader = compile ps_2_0 pixel_shader();
  }
}
