#pragma once

#include <cmath>
#include "Vector.h"

struct Box2 {
	Vec2 top_left, bottom_right;

	void join(Vec2 p) {
		if(p.y < bottom_right.y) {
			bottom_right.y = p.y;
		} else if(p.y > top_left.y) {
			top_left.y = p.y;
		}

		if(p.x < top_left.x) {
			top_left.x = p.x;
		} else if(p.x > bottom_right.x) {
			bottom_right.x = p.x;
		}
	}
};

struct iBox2 {
	iVec2 top_left, bottom_right;

	explicit iBox2(Box2 box2) : 
		top_left(std::lround(box2.top_left.x), std::lround(box2.top_left.y)),
		bottom_right(std::lround(box2.bottom_right.x), std::lround(box2.bottom_right.y)) {}
	
};