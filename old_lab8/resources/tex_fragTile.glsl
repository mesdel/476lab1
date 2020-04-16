#version 330 core
uniform sampler2D Texture0;
uniform float repeatCoeff;
uniform float ambient;

in vec2 vTexCoord;
in float dCo;
in float sCo;
out vec4 Outcolor;

void main() 
{
   vec4 texColor0 = texture(Texture0, vTexCoord * repeatCoeff);

   Outcolor = (ambient + sCo + dCo) * texColor0;
}

