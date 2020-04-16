#pragma once
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

class Collider
{
public:
	virtual void checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders) = 0;
	virtual void onCollision(shared_ptr<Collider> other) = 0;
	vec3 getPos() {
		return pos;
	}
protected:
	vec3 pos;
};

