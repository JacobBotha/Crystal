#pragma once
#include <glm/glm.hpp>

namespace Crystal {
	typedef uint16_t Index;

	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;
	};
}