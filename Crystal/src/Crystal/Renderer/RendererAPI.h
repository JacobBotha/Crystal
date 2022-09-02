#pragma once

#include <glm/glm.hpp>

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

		//TODO: Add	draw functions

		static API GetApi() { return s_API; }
		static RendererAPI* Create();
	private: 
		static API s_API;
	};
}
