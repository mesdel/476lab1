#include "TextureShader.h"

TextureShader::TextureShader(shared_ptr<Texture> t, string resourceDirectory)
{
	setVerbose(true);
	setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_fragTile.glsl");
	init();
	addUniform("P");
	addUniform("V");
	addUniform("M");
	addUniform("repeatCoeff");
	addUniform("Texture0");
	addUniform("ambient");
	addUniform("shine");
	addUniform("lightPos");
	addAttribute("vertPos");
	addAttribute("vertNor");
	addAttribute("vertTex");
	texture = t;
}

void TextureShader::bind() {
	Program::bind();
	glUniform1f(getUniform("repeatCoeff"), 10.0f);
	texture->bind(getUniform("Texture0"));

}
