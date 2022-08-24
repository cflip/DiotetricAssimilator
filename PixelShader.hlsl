Texture2D tex;
SamplerState samp;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
	return tex.Sample(samp, texCoord);
}