// �]�w���I����Ʈ榡
struct VS_INPUT
{
	float3 Position : POSITION;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

Texture2D InputTexture;
SamplerState InputTextureSampler;

// �ഫ�x�}
uniform row_major float4x4 viewproj_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// �y���ഫ
	Out.Position = mul( float4(In.Position, 1.0f), viewproj_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	return float4(1,1,1,1);
}
