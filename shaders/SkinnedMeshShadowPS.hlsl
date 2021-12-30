float fAlpha : register(c0);

float3 lightPos : register(c1);
float FarClip : register(c2);

float2 ComputeMoments(float Depth)
{
    float2 Moments;
    
    // First moment is the depth itself.   
    Moments.x = Depth;
    
    // Compute partial derivatives of depth.    
    float dx = ddx(Depth);
    float dy = ddy(Depth);
    
    // Compute second moment over the pixel extents.  
    Moments.y = Depth * Depth + 0.25 * (dx * dx + dy * dy);
    return Moments;
}

struct PS_input
{
    float2 Texcoord : TEXCOORD0;
    float2 Depth : DEPTH;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{

   float d = input.Depth.x / input.Depth.y;

    return d;
}