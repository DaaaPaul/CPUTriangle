#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "SrgbTransform.h"
#include "Vector.h"
#include "Vertex.h"
#include "Box.h"
#include "Utility.h"
#include "Shader.h"
#include <iostream>

struct RenderBuffer { 
	int w, h, ys; 
	void* data; 
};

void render(RenderBuffer& buf, int x, int y, Vec4 color) {
    uint8_t* p = reinterpret_cast<uint8_t*>(buf.data) + buf.ys * (buf.h - y - 1) + 4 * x;
    p[0] = SrgbTransform::linearToSrgb8bit(color.x);
    p[1] = SrgbTransform::linearToSrgb8bit(color.y);
    p[2] = SrgbTransform::linearToSrgb8bit(color.z);
    p[3] = std::lround(color.w * 255);
}

void draw_triangle(RenderBuffer& color_attachment, Box2 window, const Vertex::In* in_verts) {
	Vertex::Clip clip_verts[3]{};
	Vertex::NDC ndc_verts[3]{};
	Vertex::Window window_verts[3]{};

    for(int i = 0; i < 3; ++i) {
        clip_verts[i] = Shader::vertex(in_verts[i]);

		ndc_verts[i].position = perspective_divide(clip_verts[i].SV_Position);
		ndc_verts[i].texcoord = clip_verts[i].texcoord;
		ndc_verts[i].color = clip_verts[i].color;

		window_verts[i].position = 
			Vec4(mix(window.top_left.x /* 0.0f */, window.bottom_right.x /* 512.0f */, ndc_to_scale(ndc_verts[i].position.x)), 
			     mix(window.top_left.y /* 0.0f */, window.bottom_right.y /* 512.0f */, ndc_to_scale(ndc_verts[i].position.y)),
				 ndc_verts[i].position.z,
				 ndc_verts[i].position.w);
		window_verts[i].texcoord = clip_verts[i].texcoord;
		window_verts[i].color = clip_verts[i].color;
    }
	
	Box2 aabb(window_verts);
    iBox2 iaabb(aabb);
    float triangle_area = area(Vec2(window_verts[0].position), Vec2(window_verts[1].position), Vec2(window_verts[2].position));

	int xxx = 0;

    for(int y = iaabb.top_left.y; y <= iaabb.bottom_right.y; ++y) {
		for(int x = iaabb.top_left.x; x <= iaabb.bottom_right.x; ++x, ++xxx) {
			Vec4 frag_position{ x + 0.5f, y + 0.5f };

			// fragment barycentric coordinates in window coordinates
			if(xxx < 1) {
				std::cout << area(Vec2(frag_position), Vec2(window_verts[1].position), Vec2(window_verts[2].position)) << '\n';
				std::cout << area(Vec2(window_verts[0].position), Vec2(frag_position), Vec2(window_verts[2].position)) << '\n';
				std::cout << area(Vec2(window_verts[0].position), Vec2(window_verts[1].position), Vec2(frag_position)) << '\n';
			}

			Vec3 barycentric(
				area(Vec2(frag_position), Vec2(window_verts[1].position), Vec2(window_verts[2].position)) / triangle_area,
				area(Vec2(window_verts[0].position), Vec2(frag_position), Vec2(window_verts[2].position)) / triangle_area,
				area(Vec2(window_verts[0].position), Vec2(window_verts[1].position), Vec2(frag_position)) / triangle_area
			);

			// discard fragment outside the triangle. this doesn't handle edges correctly.
			if((barycentric.x + barycentric.y + barycentric.z) != 1.0f) continue;

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

			render(color_attachment, x, y, Shader::fragment(interpolated));
		}
	}
}

int main() {
	constexpr int w = 512, h = 512, ys = 512 * 4;
    RenderBuffer buffer(w, h, ys, calloc(ys, h));

    constexpr Vertex::In vertex_buffer[] = {
        { { 1.0f, -1.0f, 0.0f, 1.0f }, { 10.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 10.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -1.0f, -1.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    };

	Box2 window{ Vec2{0.0f, 0.0f}, Vec2{ static_cast<float>(buffer.w), static_cast<float>(buffer.h) } };
    draw_triangle(buffer, window, vertex_buffer);

    stbi_write_png("render/out.png", buffer.w, buffer.h, 4, buffer.data, buffer.ys);

	free(buffer.data);
}
