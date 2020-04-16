#pragma once
#include "Node3D.h"
#include "Collider.h"
#include <math.h>

class CollectNode :
	public Node3D, public Collider
{
public:
	CollectNode(shared_ptr<Shape> mesh, shared_ptr<Program> shader, vec3 position, vec3 velocity);
	virtual void update(double delta);
	void checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders);
	void onCollision(shared_ptr<Collider> other);

protected:
	vec3 pos;
	vec3 vel;
	float turnaroundTime = 3.f;
	float time = 0.f;
	float radius = 1.f;
};

