#pragma once

#include "Pipeline.h"
#include "Buffer.h"
#include "Crystal/Events/ApplicationEvent.h"

#include <glm/glm.hpp>

//struct GraphicsPipelineCreateInfo;
namespace Crystal {
	class CRYSTAL_API RendererAPI {
	public:
		enum class API {
			None = 0, Vulkan = 1
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void WindowResized(WindowResizeEvent& e) = 0;

		virtual void CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) = 0;
		virtual void DrawFrame() = 0;
		virtual void PresentFrame() = 0;
		virtual void Submit(std::shared_ptr<Buffer> vertexBuffer, uint32_t vertexCount) = 0;

		//TODO: Add	draw functions

		//Renderer API impl details
		static API GetAPI() { return s_API; }
		static RendererAPI* Create();
	private: 
		static API s_API;
	};
}
