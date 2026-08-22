#include <cassert>
#include <cmath>
#include "Utility.h"

float interpolate(Vertex::Window* p_window_verts, float Vec4::* mp_vec4, Vec3 vec3) {
	return vec3.x * p_window_verts[0].position.*mp_vec4 + vec3.y * p_window_verts[1].position.*mp_vec4 + vec3.z * p_window_verts[2].position.*mp_vec4;
}

Vec4 interpolate(Vertex::Window* p_window_verts, Vec4 Vertex::Window::* mp_window_vert, Vec3 vec3) {
	return p_window_verts[0].*mp_window_vert * vec3.x + p_window_verts[1].*mp_window_vert * vec3.y + p_window_verts[2].*mp_window_vert * vec3.z;
}

float mix(float a, float b, float scale) {
	assert(scale >= 0.0f && scale <= 1.0f);

	return a * (1.0f - scale) + b * scale;
}

float clamp(float f, float min, float max) {
	if(f > max) {
		f = max;
	} else if(f < min) {
		f = min;
	}

	return f;
}

Vec4 perspective_divide(Vec4 v) {
	return Vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1 / v.w);
}

float cross(Vec2 v1, Vec2 v2) {
	return v1.x * v2.y - v2.x * v1.y;
}

float area(Vec2 p0, Vec2 p1, Vec2 p2) { 
	return std::abs(cross(p1 - p0, p2 - p0) / 2.0f); 
};

bool equal(float f1, float f2) {
	float delta = std::abs(f1 - f2);

	return delta < 0.001f;
}

float ndc_to_scale(float ndc) {
	return 0.5f * (ndc + 1.0f);
}