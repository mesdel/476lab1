#version 330 core 
uniform vec3 difColor;
uniform vec3 ambient;
uniform float shine;
uniform vec3 specColor;
in vec3 fragNor;
in vec3 lightDir;
in vec3 viewVec;
out vec4 color;

void main()
{
	vec3 normal = normalize(fragNor);
   vec3 light = normalize(lightDir);
   vec3 eye = normalize(viewVec);
   vec3 h = (light + eye) / (length(light) + length(eye));

	color = vec4(difColor * max(dot(normal, light), 0)
              + ambient
              + specColor * pow(max(dot(normal, h), 0), shine)
              , 1.0);
}
