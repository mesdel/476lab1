#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform float shine;
// out float dCo;
// out float sCo;
out vec2 vTexCoord;
out vec3 fragNor;
out vec3 viewVec;
out vec3 lightDir;

void main()
{

   gl_Position = P * V *M * vec4(vertPos.xyz, 1.0);

   fragNor = ((M * vec4(vertNor, 0.0)).xyz);
   vec3 fragPos = (M * vec4(vertPos.xyz, 1.0)).xyz;

   // vec3 light = normalize(lightPos - fragPos);
   // vec3 eye = normalize(-1 * fragPos);
   // vec3 h = (light + eye) / (length(light) + length(eye));

   // dCo = max(dot(fragNor, light), 0);
   // sCo = pow(max(dot(fragNor, h), 0), shine);

   vTexCoord = vertTex;


	 // gl_Position = P * V * M * vertPos;
	 // fragNor = (M * vec4(vertNor, 0.0)).xyz;
	 // vec3 fragPos = (M * vertPos).xyz;
	 lightDir = lightPos - fragPos;
	 viewVec = -1 * fragPos;
 }
