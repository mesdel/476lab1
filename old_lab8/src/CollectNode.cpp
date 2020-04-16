#include "CollectNode.h"

CollectNode::CollectNode(shared_ptr<Shape> mesh, shared_ptr<Program> shader, vec3 position, vec3 velocity) : Collider(position), Node3D(mesh, shader)
{
	this->position.x = position.x;
	this->position.y = position.y;
	this->position.z = position.z;
	this->vel.x = velocity.x;
	this->vel.y = velocity.y;
	this->vel.z = velocity.z;

	cout << pos.x << ", " << pos.y << ", " << pos.z << endl;

}

void CollectNode::update(double delta) {
	Node3D::update(delta);
	position += vel * static_cast<float> (delta);
	Collider::pos.x = position.x;
	Collider::pos.y = position.y;
	Collider::pos.z = position.z;
	time += delta;
	if (time >= turnaroundTime) {
		time = 0.f;
		vel *= -1;
	}
}

void CollectNode::checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders)
{
	for (int i = 0; i < colliders->size(); i++) {
		shared_ptr<Collider> c = (*colliders)[i];
		if (c->getPos() != this->getPos()) {
			vec3 dif = c->getPos() - this->getPos();
			//cout << dif.x * dif.x + dif.y * dif.y + dif.z * dif.z << endl;
			if (dif.x * dif.x + dif.y * dif.y + dif.z * dif.z < radius * radius * 4) {
				if (vel.x != 0 || vel.y != 0 || vel.z != 0) {
					do {
						update(-.1);
						dif = c->getPos() - this->getPos();
					} while (dif.x * dif.x + dif.y * dif.y + dif.z * dif.z < radius * radius * 4);
				}
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
