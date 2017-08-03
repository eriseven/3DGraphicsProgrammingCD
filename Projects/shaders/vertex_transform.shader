// `�]�w���I����Ʈ榡`
struct VS_INPUT
{
	float4 Position : POSITION;
};

// `�]�wVertex Shader��X����Ʈ榡`
struct VS_OUTPUT
{
	float4 Position : POSITION;
};

// `�ഫ�x�}`
uniform row_major float4x4 viewproj_matrix : register(c0);
uniform float4x4 viewproj_matrix_t : register(c0);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	// `�y���ഫ`
	Out.Position = mul( In.Position, viewproj_matrix);
	// `����x�}���� row major ��, ���k�n�ϹL�Ӱ�.`
	// Out.Position = mul( viewproj_matrix_t, In.Position );
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// `�Ǧ^�զ�, �û��e�@�ӥ��I.`
	return float4(1,1,1,1);
}
