#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "SrgbTransform.h"
#include "Vector.h"
#include "Vertex.h"
#include "Box.h"
#include "Utility.h"
#include <cmath>
#include <cassert>
#include <utility>

struct RenderBuffer { 
	int w, h, ys; 
	void* data; 
};

Vertex::Clip vertex_shader(Vertex::In in) {
	Vertex::Clip out{};

	out.SV_Position = Vec4(in.position.x, in.position.y, -2 * in.position.z - 2 * in.position.w, -in.position.z);
    out.texcoord = in.texcoord;
    out.color = in.color;

	return out;
}

Vec4 fragment_shader(Vertex::Interpolated in) {
    Vec4 out = in.color;
	
	Vec2 wrapped = Vec2(in.texcoord.x, in.texcoord.y) - Vec2(std::floor(in.texcoord.x), std::floor(in.texcoord.y));
    bool brighter = (wrapped.x < 0.5) != (wrapped.y < 0.5);
    
	if(!brighter) {
		out.x *= 0.5f;
		out.y *= 0.5f;
		out.z *= 0.5f;
	}

	return out;
}

// render output unit/render operations pipeline
void render(RenderBuffer& buf, int x, int y, Vec4 color) {
    uint8_t* p = reinterpret_cast<uint8_t*>(buf.data) + buf.ys * (buf.h - y - 1) + 4 * x;
    p[0] = SrgbTransform::linearToSrgb8bit(color.x);
    p[1] = SrgbTransform::linearToSrgb8bit(color.y);
    p[2] = SrgbTransform::linearToSrgb8bit(color.z);
    p[3] = std::lround(color.w * 255);
}

void draw_triangle(RenderBuffer& color_attachment, Box2 viewport, const Vertex::In* in_verts) {
	Vertex::Clip clip_verts[3]{};
	Vertex::NDC ndc_verts[3]{};
	Vertex::Window window_verts[3]{};
    Box2 aabb{ Vec2{}, Vec2{} };

    for(int i = 0; i < 3; ++i) {
        clip_verts[i] = vertex_shader(in_verts[i]);

		ndc_verts[i].position = perspective_divide(clip_verts[i].SV_Position);
		ndc_verts[i].texcoord = clip_verts[i].texcoord;
		ndc_verts[i].color = clip_verts[i].color;

		window_verts[i].position = 
			Vec4(mix(viewport.top_left.x, viewport.bottom_right.x, ndc_to_scale(ndc_verts[i].position.x)), 
			     mix(viewport.top_left.y, viewport.bottom_right.y, ndc_to_scale(ndc_verts[i].position.y)),
				 ndc_verts[i].position.z,
				 ndc_verts[i].position.w);
		window_verts[i].texcoord = clip_verts[i].texcoord;
		window_verts[i].color = clip_verts[i].color;

        aabb.join(Vec2(window_verts[i].position));
    }

    float triangle_area = area(Vec2(window_verts[0].position), Vec2(window_verts[1].position), Vec2(window_verts[2].position));
    iBox2 iaabb(aabb);

    for(int y = iaabb.top_left.y; y >= iaabb.bottom_right.y; --y) {
		for(int x = iaabb.top_left.x; x <= iaabb.bottom_right.x; ++x) {
			Vec4 frag_position(x + 0.5f, y + 0.5f, 0.0f, 0.0f);

			// fragment barycentric coordinates in window coordinates
			Vec3 barycentric(
				area(Vec2(frag_position), Vec2(window_verts[1].position), Vec2(window_verts[2].position)) / triangle_area,
				area(Vec2(window_verts[0].position), Vec2(frag_position), Vec2(window_verts[2].position)) / triangle_area,
				area(Vec2(window_verts[0].position), Vec2(window_verts[1].position), Vec2(frag_position)) / triangle_area
			);

			// discard fragment outside the triangle. this doesn't handle edges correctly.
			if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

			// interpolate inverse depth linearly
			frag_position.z = interpolate(window_verts, &Vec4::z, barycentric);
			frag_position.w = interpolate(window_verts, &Vec4::w, barycentric);

			// clip fragments to the near/far planes
			if(frag_position.z < 0 || frag_position.z > 1) continue;

			// convert to perspective correct (clip-space) barycentric
			Vec3 perspective = barycentric * Vec3(window_verts[0].position.w, window_verts[1].position.w, window_verts[2].position.w) * (1 / frag_position.w);

			// interpolate attributes
			Vertex::Interpolated interpolated{
				interpolate(window_verts, &Vertex::Window::position, perspective),
				interpolate(window_verts, &Vertex::Window::texcoord, perspective),
				interpolate(window_verts, &Vertex::Window::color, perspective),
			};

			render(color_attachment, x, y, fragment_shader(interpolated));
		}
	}
}

int main() {
	constexpr int w = 512, h = 512, ys = 512 * 4;
    RenderBuffer buffer(w, h, ys, calloc(ys, h));

    // position, texcoord, color
    constexpr Vertex::In vertex_buffer[] = {
        { { 1, -1, -1, 1 }, { 10, 0, 0, 1 }, { 1, 0, 0, 1 } },
        { { 0, 1, -1, 1 }, { 0, 10, 0, 1 }, { 0, 1, 0, 1 } },
        { { -1, -1, -2, 1 }, { 0, 0, 0, 1 }, { 0, 0, 1, 1 } },
    };

    Box2 viewport(Vec2(0, 0), Vec2(buffer.w, buffer.h));
    draw_triangle(buffer, viewport, vertex_buffer);

    stbi_write_png("render/out.png", buffer.w, buffer.h, 4, buffer.data, buffer.ys);

	free(buffer.data);
}
