//Constant Buffer Variables

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//Vertex Shader
VOut VS(float4 position : POSITION, float4 color : COLOR)
{
    VOut output = (VS_OUTPUT)0;
	
   	output.position = mul(position, World);
   	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.color = color;

    return output;
}

//Pixel Shader
float4 PS(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}

//technique technique0 {
//	pass p0 {
		
//	}
}
