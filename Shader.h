#pragma once

#include "Vertex.h"
#include "Vector.h"

namespace Shader {
	Vertex::Clip vertex(Vertex::In in);
	Vec4 fragment(Vertex::Interpolated in);
}