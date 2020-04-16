#include "CollectNode.h"

CollectNode::CollectNode(shared_ptr<Shape> mesh, shared_ptr<Program> shader, vec3 position, vec3 velocity) : Node3D(mesh, shader)
{
	this->position = position;
	this->pos = position;
	this->vel = velocity;
}

void CollectNode::update(double delta) {
	Node3D::update(delta);
	position += vel * (float) delta;
	pos = position;
	time += delta;
	if (time >= turnaroundTime) {
		time = 0.f;
		vel *= -1;
	}
}

void CollectNode::checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders)
{
	for (int i = 0; i < colliders->size(); i++) {
		if ((*colliders)[i]->getPos() != this->pos) {
			if (distance((*colliders)[i]->getPos(), this->pos) <= 2 * radius) {
				cout << "colllision" << endl;
				onCollision((*colliders)[i]);
				return;
				//(*colliders)[i]->onCollision(make_shared<CollectNode>(this));
			}
		}
	}
}

void CollectNode::onCollision(shared_ptr<Collider> other)
{
	vel = -vel;
	time = turnaroundTime - time;
}
