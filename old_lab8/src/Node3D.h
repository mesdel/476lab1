#pragma once
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "MatrixStack.h"
#include "Shape.h"
#include "Program.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"
#include "Texture.h"
#include "tiny_obj_loader/tiny_obj_loader.h"
#include <unordered_map>

using namespace std;
using namespace glm;
using namespace tinyobj;

class Node3D
{
public:
	Node3D(shared_ptr<Shape> mesh, shared_ptr<Program> shader, shared_ptr<material_t> mat = nullptr);
	Node3D(shared_ptr<vector<shared_ptr<Shape>>> meshes, shared_ptr<Program> shader, int rootMesh = 0, shared_ptr<material_t> mat = nullptr);
	void draw(shared_ptr<MatrixStack> model);
	void setPos(vec3 p);
	void setScale(vec3 s);
	void setRot(vec3 r);
	void setPOffset(vec3 o);
	void setPAnchor(vec3 a); // set offset based on percentage of mesh BBox
	void setROffset(vec3 o);
	void setRAnchor(vec3 a);
	vec3 getOffset();
	void addChild(shared_ptr<Node3D> child, bool sameObj = false);
	bool isDead() {
		return dead;
	}
	virtual void update(double delta);

protected:
	shared_ptr<Program> shader;
	vector<shared_ptr<Node3D>> uniqueChildren;
	vector<shared_ptr<Node3D>> sameObjChildren;
	shared_ptr<Shape> mesh;
	string meshName = "SmoothSphere.obj0";
	bool visible = true;
	vec3 rOffset = vec3(0.0, 0.0, 0.0);
	vec3 pOffset = vec3(0.0, 0.0, 0.0);
	vec3 position = vec3(0.0, 0.0, 0.0);
	vec3 rotation = vec3(0.0, 0.0, 0.0);
	vec3 scale = vec3(1.0, 1.0, 1.0);
	shared_ptr<material_t> mat;
	bool dead = false;
};

