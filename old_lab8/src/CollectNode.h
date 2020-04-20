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
	void onCollision(shared_ptr<Collider> other = nullptr);
	void kill();
	static const float BOARD_WIDTH;
protected:
	vec3 pos;
	vec3 vel;
	float turnRate = 2.f;
	float turnAmount = .0f;
	bool turning = false;
	float radius = 1.f;
	bool shrink = false;
};

