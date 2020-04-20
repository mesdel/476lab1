#include "CollectNode.h"

#define BOARD_WIDTH 20

CollectNode::CollectNode(shared_ptr<Shape> mesh, shared_ptr<Program> shader, vec3 position, vec3 velocity) : Collider(position), Node3D(mesh, shader)
{
	//this->setPAnchor(vec3(.5));
	this->position.x = position.x;
	this->position.y = position.y;
	this->position.z = position.z;
	Collider::pos = this->position;
	this->vel.x = velocity.x;
	this->vel.y = velocity.y;
	this->vel.z = velocity.z;
	rotation.y = atan2(vel.z, vel.x);

}

void CollectNode::update(double delta) {
	if (dead) {
		return;
	}
	Node3D::update(delta);
	if (shrink) {
		scale -= vec3(.3 * static_cast<float>(delta));
		if (scale.x <= .0) {
			dead = true;
		}
		return;
	}
	if (turning) {
		float t = static_cast<float>(delta) * turnRate;
		rotation.y += t;
		turnAmount += t;
		if (turnAmount >= 3.14159) {
			rotation.y -= turnAmount - 3.14159;
			turning = false;
			turnAmount = 0;
		}
		return;
	}
	position += vel * static_cast<float> (delta);
	if (position.x < -BOARD_WIDTH) {
		position.x = -BOARD_WIDTH;
		//vel.x = -vel.x;
		if (!turning) {
			turning = true;
			vel = -vel;
		}
	}
	else if (position.x > BOARD_WIDTH) {
		position.x = BOARD_WIDTH;
		//vel.x = -vel.x;
		if (!turning) {
			turning = true;
			vel = -vel;
		}
	}
	else if (position.z < -BOARD_WIDTH) {
		position.z = -BOARD_WIDTH;
		//vel.z = -vel.z;
		if (!turning) {
			turning = true;
			vel = -vel;
		}
	}
	else if (position.z > BOARD_WIDTH) {
		position.z = BOARD_WIDTH;
		//vel.z = -vel.z;
		if (!turning) {
			turning = true;
			vel = -vel;
		}
	}
	Collider::pos.x = position.x;
	Collider::pos.y = position.y;
	Collider::pos.z = position.z;
	
}

void CollectNode::checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders)
{
	for (int i = 0; i < colliders->size(); i++) {
		shared_ptr<Collider> c = (*colliders)[i];
		if (c->getPos() != this->getPos()) {
			vec3 dif = c->getPos() - this->getPos();
			//cout << dif.x * dif.x + dif.y * dif.y + dif.z * dif.z << endl;
			float d = sqrt(dif.x * dif.x + dif.y * dif.y + dif.z * dif.z);
			if (d < radius * 2) {
				position -= dif * (radius * 2 - d);
				Collider::pos = position;
				/*if (vel.x != 0 || vel.y != 0 || vel.z != 0) {
					do {
						update(-.1);
						dif = c->getPos() - this->getPos();
					} while (dif.x * dif.x + dif.y * dif.y + dif.z * dif.z < radius * radius * 4);
				}*/
				onCollision();
				(*colliders)[i]->onCollision();
				return;
				//(*colliders)[i]->onCollision(make_shared<CollectNode>(this));
			}
		}
	}
}

void CollectNode::onCollision(shared_ptr<Collider> other)
{
	if (!turning) {
		turning = true;
		vel = -vel;
	}
}

void CollectNode::kill()
{
	shrink = true;
}
