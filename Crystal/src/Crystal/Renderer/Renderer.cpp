#include "clpch.h"

#include "Renderer.h"

namespace Crystal {
	std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = std::unique_ptr<RendererAPI>(RendererAPI::Create());

	void Renderer::Init() {
		s_RendererAPI->Init();
	}

	void Renderer::Shutdown() {
		s_RendererAPI.reset();
	}

	std::shared_ptr<Shader> Renderer::CreateShader(std::string fileName) {
		return Shader::Create(fileName, s_RendererAPI.get());
	}
}