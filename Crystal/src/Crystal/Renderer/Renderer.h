#pragma once

#include "RendererAPI.h"
#include "Shader.h"
#include <glm/glm.hpp>

class Shader;
namespace Crystal {
	class CRYSTAL_API Renderer {
	public:
		static void Init();
		static void Shutdown();
		
		static std::shared_ptr<Shader> CreateShader(std::string fileName);
		static RendererAPI::API GetAPI() { return s_RendererAPI->GetAPI(); }
		//TODO: Functions for handling scene and submitting vertex/shader data
	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
		static std::unique_ptr<RendererAPI> s_RendererAPI;
	};
}
