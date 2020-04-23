#version 330 core
uniform sampler2D Texture0;
uniform float repeatCoeff;
uniform float ambient;
uniform vec3 specColor;
uniform float shine;

in vec2 vTexCoord;
in vec3 lightDir;
in vec3 fragNor;
in vec3 viewVec;
// in float dCo;
// in float sCo;
out vec4 Outcolor;

void main()
{
	// vec4 texColor0 = texture(Texture0, vTexCoord * repeatCoeff);
	// Outcolor = (ambient + sCo + dCo) * texColor0;

	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 eye = normalize(viewVec);
	vec3 h = (light + eye) / (length(light) + length(eye));

	Outcolor = vec4(texture(Texture0, vTexCoord * repeatCoeff).xyz * max(dot(normal, light), ambient) + specColor * pow(max(dot(normal, h), 0), shine), 1.0);
}
