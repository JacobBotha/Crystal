#pragma once

#include "RendererAPI.h"
#include <glm/glm.hpp>

namespace Crystal {
	class CRYSTAL_API Renderer {
	public:
		static void Init();
		static void Shutdown();
		
		static RendererAPI* GetRenderAPI() { return s_RendererAPI.get(); }
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
