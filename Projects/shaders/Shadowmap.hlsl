// �p�⳱�v�ĪG

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 LightPos : TEXCOORD0;
	float3 Normal   : TEXCOORD1;
};

// �ഫ�x�}
uniform row_major float4x4 wvp_matrix		: register(c0);
uniform row_major float4x4 light_wvp_matrix : register(c4);
uniform row_major float4x4 light_wv_matrix	: register(c8);

sampler2D texShadowmapSampler : register(s0);

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.LightPos = mul( float4(In.Position, 1.0f), light_wvp_matrix);
	Out.Normal =  mul( In.Normal.xyz, (float3x3) light_wv_matrix);
	
	return Out;
}

//
// Pixel Shader
// �ϥ�ZBuffer�ʺA�K�Ϩӭp�⳱�v
float4 PS_D24S8(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	
	// �ϥ�DepthBuffer����J�K�Ϯ�, �|�۰ʰ�PS_R32F�禡�̬ۦP���Z�����.
	float4 Lit = tex2Dproj(texShadowmapSampler, In.LightPos);
	float4 color = fSpotLightEffect * In.Normal.z * Lit;
	
	return color;
}

//
// Pixel Shader
// �ϥ�Float32�ʺA�K�Ϩӭp�⳱�v
float4 PS_R32F(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	//projected_pos.xy += float2(0.5f/1024.0f, 0.5f/1024.f);
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);

	float4 shadowmap = tex2D(texShadowmapSampler, projected_pos.xy);
	float  fLit = projected_pos.z < shadowmap.r;
	float4 color = fSpotLightEffect * In.Normal.z * fLit;

	return color;
}
