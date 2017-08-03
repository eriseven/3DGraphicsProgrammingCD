// �]�w���I����Ʈ榡
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Texcoord : TEXCOORD0;
};

TextureCube InputTexture;
SamplerState InputTextureSampler;

// �ഫ�x�}
uniform row_major float4x4 viewproj_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	// �y���ഫ
	Out.Position = mul( float4(In.Position.xyz, 1), viewproj_matrix);
	// ��Normal�������K�Ϯy��
	Out.Texcoord = In.Normal;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 color = InputTexture.Sample(InputTextureSampler, In.Texcoord);
	return color;
}
