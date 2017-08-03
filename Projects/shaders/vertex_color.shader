// �]�w���I����Ʈ榡
struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
};

// �ഫ�x�}
uniform row_major float4x4 viewproj_matrix : register(c0);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// �y���ഫ
	Out.Position = mul( float4(In.Position.xyz, 1.0f), viewproj_matrix);
	// Ū�����I���C��
	Out.Color = In.Color;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// �ϥγ��I�������X�Ӫ��C��
	return In.Color;
}
