#pragma once

struct Vec4 {
	float x, y, z, w;

	Vec4 operator*(float f) const {
		return Vec4(x * f, y * f, z * f, w * f);
	}

	Vec4 operator+(Vec4 vec4) const {
		return Vec4(x + vec4.x, y + vec4.y, z + vec4.z, w + vec4.w);
	}
};

struct Vec3 {
	float x, y, z;

	Vec3 operator*(float f) const {
		return Vec3(x * f, y * f, z * f);
	}

	Vec3 operator*(Vec3 v) const {
		return Vec3(x * v.x, y * v.y, z * v.z);
	}
};

struct Vec2 {
	float x, y;

	Vec2() = default;
	Vec2(float x, float y) : x(x), y(y) {}
	explicit Vec2(Vec4 vec4) : x(vec4.x), y(vec4.y) {}

	Vec2 operator-(Vec2 v) const {
		return Vec2(x - v.x, y - v.y);
	}
};

struct iVec2 {
	int x, y;
};