#include "shared.fx"
#include "terrainShared.fx"

struct VS_OUTPUT
{
    float4  pos		: POSITION;
    float2  uv		: TEXCOORD0;
    float3  LightDir: TEXCOORD2;
	float3  ViewDir	: TEXCOORD3;
    float4  color	: COLOR;
};

VS_OUTPUT VS(VS_TERRAIN_INPUT i)
{
	VS_OUTPUT o;
	o.uv = i.uv*g_fScaleUV;
	float3 vBinormal	= normalize(cross(float3(0,0,1),i.normal));//normalize(cross(i.normal,float3(0,0,1)));
	float3 vTangent		= normalize(cross(i.normal,float3(1,0,0)));//normalize(cross(float3(1,0,0),i.normal));
	float3x3 mTangent = transpose(float3x3(vBinormal,i.normal,vTangent));
	o.LightDir = normalize(mul(mTangent,-g_vLightDir));
	o.ViewDir= normalize(mul(mTangent,i.pos.xyz-g_vEyePot));
	o.pos = mul(i.pos,g_mViewProj);
	o.color = i.color;
	return o;
}

float4 PS(VS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(g_samDiffuse, i.uv);
	float3 cLight	= i.color.rgb;//tex2D(g_samLight, i.UV1);
	float3 vNormal	= normalize(tex2D(g_samNormal, i.uv).xzy-0.5);
	//vNormal.z=-vNormal.z;
	float4 color;
	color.a = i.color.a;
	color.xyz = saturate(dot(vNormal, i.LightDir))*0.8+0.2;
	color.xyz*=cLight;
		//return color;
	color *= cDiffuse;

	{
		float3 cSpecular= tex2D(g_samSpecular, i.uv).xyz;
		float3 reflVec = normalize(reflect(normalize(i.ViewDir), vNormal));
		float fSpec = pow(saturate(dot(reflVec, normalize(i.LightDir))),32)*cLight;
		//reflVec = normalize(reflect(-reflVec, normalize(i.LightDir)));
		//float3 cEnvironment = texCUBE(g_samEnvironment, reflVec-i.LightDir).xyz;
		//color.xyz +=cEnvironment*cSpecular;
		color.xyz +=cSpecular*fSpec;
	}
	return color;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}