// ���I����Ʈ榡
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color	: COLOR;
};

uniform row_major float4x4 g_worldviewproj_matrix : register(c0);
uniform row_major float4x4 g_world_matrix : register(c4);

// ���ҥ�
uniform float4 g_Ambient : register(c8);
// ��V��
uniform float4 g_Light0_Direction : register(c9);
uniform float4 g_Light0_Ambient : register(c10);
uniform float4 g_Light0_Diffuse : register(c11);
uniform float4 g_Light0_Specular : register(c12);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// �y���ഫ
	Out.Position = mul( In.Position, g_worldviewproj_matrix);
	float3 vWorld_Position = mul( In.Position, g_world_matrix);
	float3 vWorld_Normal = mul( In.Normal, (float3x3) g_world_matrix);
	
	// ���	
	float4 vLighting = g_Ambient + g_Light0_Ambient;
	// ��V��
	vLighting += g_Light0_Diffuse * saturate(dot(g_Light0_Direction, vWorld_Normal));
		
	Out.Color = vLighting;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	// �ϥγ��I�������X�Ӫ��C��
	return In.Color;
}
