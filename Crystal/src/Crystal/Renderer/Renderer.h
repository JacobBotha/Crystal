#pragma once

#include "RendererAPI.h"
#include "Crystal/Events/ApplicationEvent.h"
#include "Buffer.h"

#include <glm/glm.hpp>

//class Shader;
namespace Crystal {
	class CRYSTAL_API Renderer 
	{
	public:
		static void Init();
		static void Shutdown();

		static void WindowResized(WindowResizeEvent& e) { s_RendererAPI->WindowResized(e); }
		static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewPort(x, y, width, height); }
		static void SetClearColor(glm::vec4& color) { s_RendererAPI->SetClearColor(color); }


		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		static RendererAPI* GetRendererAPI() { return s_RendererAPI.get(); }

		//TODO: Functions for handling scene and submitting vertex/shader data
		static void CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) { return s_RendererAPI->CreateGraphicsPipeline(createInfo); }
		static void DrawFrame() { return s_RendererAPI->DrawFrame(); }

		static void PresentFrame() { return s_RendererAPI->PresentFrame(); }
		
		static void Submit(std::shared_ptr<Buffer> vertexBuffer, uint32_t vertexCount, std::shared_ptr<Buffer> indexBuffer, uint32_t indexCount) 
		{ 
			CL_CORE_ASSERT(vertexBuffer->GetType() == Buffer::BufferType::Vertex, "RenderSubmit must be a vertex buffer.");
			return s_RendererAPI->Submit(vertexBuffer, vertexCount, indexBuffer, indexCount); 
		}

		static void Clear() { return s_RendererAPI->Clear(); };
	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
		static std::unique_ptr<RendererAPI> s_RendererAPI;
	};
}
