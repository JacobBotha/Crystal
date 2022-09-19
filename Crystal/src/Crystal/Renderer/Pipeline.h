#pragma once

#include "Shader.h"

class Shader;
namespace Crystal {
	struct GraphicsPipelineCreateInfo {
		Shader* vertexShader;
		Shader* fragmentShader;
		bool dynamicViewport = true;
		bool fillPolygon = true;
	};
}
