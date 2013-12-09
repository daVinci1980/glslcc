/*
int TextureDimensions1D ( sampler1D samp , int nMipLevel ) 
{ 
    return textureSize ( samp , nMipLevel ); 
} 

int TextureDimensionsCube ( samplerCube samp , int nMipLevel ) 
{ 
    return textureSize ( samp , nMipLevel ) . x ; 
} 
*/

struct PS_OUTPUT
{
    vec4 vColor;
};

struct PS_INPUT
{
    vec4 vTexCoord;
    vec4 vColor;
};

uniform sampler2D g_tColor;

PS_OUTPUT MainPs ( PS_INPUT i ) 
{ 
    PS_OUTPUT o ; 
    
    vec4 vColor = texture2D ( g_tColor , ( i . vTexCoord . xy ) . xy ) ; 
    

    
    
    // o . vColor = i . vColor . rgba * vColor . rgba ; 
    return o ; 
} 
