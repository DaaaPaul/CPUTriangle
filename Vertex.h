#pragma once

#include "Vector.h"

namespace Vertex {
	struct In { 
		Vec4 position, texcoord, color; 
	};

	struct Clip {
		Vec4 SV_Position, texcoord, color;
	};

	struct NDC {
		Vec4 position, texcoord, color;
	};

	struct Window {
		Vec4 position, texcoord, color;
	};

	struct Interpolated {
		Vec4 interpolated, texcoord, color;
	};
}