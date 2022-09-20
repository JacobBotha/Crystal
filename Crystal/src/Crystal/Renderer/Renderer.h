#pragma once

#include "RendererAPI.h"
#include "Crystal/Events/ApplicationEvent.h"
//#include "Shader.h"
#include <glm/glm.hpp>

//class Shader;
namespace Crystal {
	class CRYSTAL_API Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) { return s_RendererAPI->CreateGraphicsPipeline(createInfo); }

		static void DrawFrame() { return s_RendererAPI->DrawFrame(); }

		static void WindowResized(WindowResizeEvent& e) { s_RendererAPI->WindowResized(e); }

		static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewPort(x, y, width, height); }
		
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
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
