#pragma once
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;

class Collider
{
public:
	Collider(vec3 position);
	virtual void checkCollisions(shared_ptr<vector<shared_ptr<Collider>>> colliders) = 0;
	virtual void onCollision(shared_ptr<Collider> other = nullptr) = 0;
	virtual void kill() = 0;
	vec3 getPos() {
		return pos;
	}
	float getRadius() {
		return 1.f;
	}
protected:
	vec3 pos = vec3(0, 0, 0);
};

