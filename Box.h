#pragma once

#include <cmath>
#include <limits>
#include "Vector.h"
#include "Vertex.h"

struct Box2 {
	Vec2 top_left, bottom_right;

	Box2(Vec2 top_left, Vec2 bottom_right) : top_left(top_left), bottom_right(bottom_right) {}
	explicit Box2(const Vertex::Window* triangle) {
		float max_x = std::numeric_limits<float>::min();
		float max_y = std::numeric_limits<float>::min();

		float min_x = std::numeric_limits<float>::max();
		float min_y = std::numeric_limits<float>::max();

		for(int i = 0; i < 3; ++i) {
			if(triangle[i].position.x > max_x) {
				max_x = triangle[i].position.x;
			}
			if(triangle[i].position.x < min_x) {
				min_x = triangle[i].position.x;
			}
			if(triangle[i].position.y > max_y) {
				max_y = triangle[i].position.y;
			}
			if(triangle[i].position.y < min_y) {
				min_y = triangle[i].position.y;
			}
		}

		top_left.x = min_x;
		top_left.y = min_y;

		bottom_right.x = max_x;
		bottom_right.y = max_y;
	}
};

struct iBox2 {
	iVec2 top_left, bottom_right;

	explicit iBox2(Box2 box2) : 
		top_left(std::lround(box2.top_left.x), std::lround(box2.top_left.y)),
		bottom_right(std::lround(box2.bottom_right.x), std::lround(box2.bottom_right.y)) {}
	
};