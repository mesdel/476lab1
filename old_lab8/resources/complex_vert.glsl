#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
out vec3 fragNor;
out vec3 lightDir;
out vec3 viewVec;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragNor = (M * vec4(vertNor, 0.0)).xyz;
   vec3 fragPos = (M * vertPos).xyz;
   lightDir = lightPos - fragPos;
   viewVec = -1 * fragPos;
}
