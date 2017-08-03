// �]�w���I����Ʈ榡
struct VS_INPUT
{
	float4 Position : POSITION;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : POSITION;
};

// �ഫ�x�}
uniform row_major float4x4 viewproj_matrix : register(c0);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	// �y���ഫ
	Out.Position = mul( In.Position, viewproj_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// �Ǧ^�զ�A�û��e�@�ӥ��I�C
	return float4(1,1,1,1);
}
