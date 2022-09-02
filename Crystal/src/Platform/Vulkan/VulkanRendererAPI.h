#pragma once

#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal {
	class CRYSTAL_API VulkanRendererAPI : RendererAPI {
	public:
		VulkanRendererAPI() {}
		~VulkanRendererAPI() {}

		virtual void Init() override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
	};
}
