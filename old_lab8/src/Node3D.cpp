#include "Node3D.h"

Node3D::Node3D(shared_ptr<Shape> mesh, shared_ptr<Program> shader, shared_ptr<material_t> mat)
{
	this->mesh = mesh;
	this->shader = shader;
	this->mat = mat;
}

Node3D::Node3D(shared_ptr<vector<shared_ptr<Shape>>> meshes, shared_ptr<Program> shader, int rootMesh, shared_ptr<material_t> mat)
{
	this->mesh = (*meshes)[rootMesh];
	for (int i = 0; i < meshes->size(); i++) {
		if (i == rootMesh) {
			continue;
		}
		shared_ptr<Node3D> n = make_shared<Node3D>((*meshes)[i], shader, mat);
		sameObjChildren.push_back(n);
	}
	this->shader = shader;
	this->mat = mat;
}

void Node3D::draw(shared_ptr<MatrixStack> model)
{
	model->pushMatrix();

	//place at position
	model->translate(position + rOffset * scale);

	//rotate
	model->rotate(rotation.y, vec3(0, 1, 0));
	model->rotate(rotation.z, vec3(0, 0, 1));
	model->rotate(rotation.x, vec3(1, 0, 0));

	//move to rotational anchor
	model->translate(-rOffset * scale);

	//cout << "drawing " << uniqueChildren.size() <<  " children" << endl;
	for (int i = 0; i < uniqueChildren.size(); i++) {
		uniqueChildren[i]->draw(model);
	}

	//non-uniform scale
	model->scale(scale);
	//positional anchor
	model->translate(-pOffset);

	for (int i = 0; i < sameObjChildren.size(); i++) {
		sameObjChildren[i]->draw(model);
	}
	shader->bind();
	if (mesh != NULL && visible) {
		glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(model->topMatrix()));
		mesh->draw(shader);
	}
	model->popMatrix();
}

void Node3D::setPos(vec3 p)
{
	position.x = p.x;
	position.y = p.y;
	position.z = p.z;
}

void Node3D::setScale(vec3 s)
{
	scale.x = s.x;
	scale.y = s.y;
	scale.z = s.z;
}

void Node3D::setRot(vec3 r)
{
	rotation.x = r.x;
	rotation.y = r.y;
	rotation.z = r.z;
}

void Node3D::setPOffset(vec3 o) {
	pOffset.x = o.x;
	pOffset.y = o.y;
	pOffset.z = o.z;
}

void Node3D::setPAnchor(vec3 a)
{
	//map offset as percentage of max, 0.0 to 1.0;
	pOffset.x = a.x * (mesh->max.x - mesh->min.x) + mesh->min.x;
	pOffset.y = a.y * (mesh->max.y - mesh->min.y) + mesh->min.y;
	pOffset.z = a.z * (mesh->max.z - mesh->min.z) + mesh->min.z;
}

void Node3D::setROffset(vec3 o) {
	rOffset.x = o.x;
	rOffset.y = o.y;
	rOffset.z = o.z;
}

void Node3D::setRAnchor(vec3 a)
{
	//map offset as percentage of max, 0.0 to 1.0;
	rOffset.x = a.x * (mesh->max.x - mesh->min.x) + mesh->min.x;
	rOffset.y = a.y * (mesh->max.y - mesh->min.y) + mesh->min.y;
	rOffset.z = a.z * (mesh->max.z - mesh->min.z) + mesh->min.z;
}

vec3 Node3D::getOffset() {
	return pOffset;
}

void Node3D::addChild(shared_ptr<Node3D> child, bool sameObj)
{

	if (sameObj) {
		child->setPOffset(vec3(0, 0, 0)); //this is relevent if the child is a part of the same object, otherwise we don't want
		child->setROffset(vec3(0, 0, 0));
		sameObjChildren.push_back(child);
	}
	else {
		uniqueChildren.push_back(child);
	}
}

void Node3D::update(double delta)
{
	//rotation.y += delta;
	for (int i = 0; i < sameObjChildren.size(); i++) {
		sameObjChildren[i]->update(delta);
	}
	for (int i = 0; i < uniqueChildren.size(); i++) {
		uniqueChildren[i]->update(delta);
	}
}