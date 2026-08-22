#include <cmath>
#include "Vertex.h"

namespace Shader {
	Vertex::Clip vertex(Vertex::In in) {
		return {
			in.position,
			in.texcoord,
			in.color
		};
	}

	Vec4 fragment(Vertex::Interpolated in) {
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
}