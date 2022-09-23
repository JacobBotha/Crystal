#pragma once

#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Core/Window.h"

namespace Crystal {
	class ImGuiRenderer {
	public:
		virtual ~ImGuiRenderer() = default;
		virtual void Init(RendererAPI* rendererAPI, Window* window) = 0;
		virtual void NewFrame() = 0;
		virtual void Shutdown() = 0;
		virtual void Render(void* drawData) = 0;

		static std::unique_ptr<ImGuiRenderer> Create();
	};
}
