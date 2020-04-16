#pragma once
#include "Program.h"
#include "Texture.h"
#include <iostream>

using namespace std;
class TextureShader :
	public Program
{
public:
	TextureShader(shared_ptr<Texture> t, string resourceDirectory = "../resources");
	virtual void bind();
protected:
	shared_ptr<Texture> texture;
};

