#pragma once

#include "Vector.h"
#include "Vertex.h"

float interpolate(Vertex::Window* p_window_verts, float Vec4::* mp_vec4, Vec3 vec3);

Vec4 interpolate(Vertex::Window* p_window_verts, Vec4 Vertex::Window::* mp_window_vert, Vec3 vec3);

float mix(float a, float b, float scale);

float clamp(float f, float min, float max);

Vec4 perspective_divide(Vec4 v);

float cross(Vec2 v1, Vec2 v2);

float area(Vec2 p0, Vec2 p1, Vec2 p2);

bool equal(float f1, float f2);

float ndc_to_scale(float ndc);