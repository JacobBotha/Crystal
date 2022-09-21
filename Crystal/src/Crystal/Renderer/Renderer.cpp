#include "clpch.h"

#include "Crystal/Core/Application.h"
#include "Renderer.h"

namespace Crystal {
	std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = std::unique_ptr<RendererAPI>(nullptr);

	void Renderer::Init() {
		s_RendererAPI = std::unique_ptr<RendererAPI>(RendererAPI::Create());
		s_RendererAPI->Init();
	}

	void Renderer::Shutdown() {
		s_RendererAPI.reset();
	}
}