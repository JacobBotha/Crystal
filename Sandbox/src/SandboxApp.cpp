#include <Crystal.h>

class ExampleLayer : public Crystal::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//Crystal::Renderer::DrawFrame();
	}

	void OnEvent(Crystal::Event& event) override
	{
		if(event.GetEventType() == Crystal::EventType::KeyPressed) {
			//Convert event to key event
			Crystal::KeyPressedEvent* kEvent = (Crystal::KeyPressedEvent*) &event;
			//Check if Tab key is pressed and reload shaders/graphics pipeline
			if (kEvent->GetKeyCode() == Crystal::Key::Tab) {
				m_VertShader = Crystal::Shader::Create("assets/shaders/shader.vert");
				m_FragShader = Crystal::Shader::Create("assets/shaders/shader.frag");
				Crystal::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
				graphicsPipelineCreateInfo.vertexShader = m_VertShader.get();
				graphicsPipelineCreateInfo.fragmentShader = m_FragShader.get();
				Crystal::Renderer::CreateGraphicsPipeline(graphicsPipelineCreateInfo);
			}
		}
		CL_TRACE("{0}", event);
	}

	void OnAttach() override {
		m_VertShader = Crystal::Shader::Create("assets/shaders/shader.vert");
		m_FragShader = Crystal::Shader::Create("assets/shaders/shader.frag");
		
		Crystal::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.vertexShader = m_VertShader.get();
		graphicsPipelineCreateInfo.fragmentShader = m_FragShader.get();
		Crystal::Renderer::CreateGraphicsPipeline(graphicsPipelineCreateInfo);

		glm::vec4 color{};
		color.r = 0.3f;
		color.g = 0.3f;
		color.b = 0.3f;
		color.a = 1.0f;
		Crystal::Renderer::SetClearColor(color);
	}

	void OnDetach() override {
		m_VertShader.reset();
		m_FragShader.reset();
	}

	std::shared_ptr<Crystal::Shader> m_VertShader;
	std::shared_ptr<Crystal::Shader> m_FragShader;

};

class Sandbox : public Crystal::Application
{
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
	~Sandbox() {}
};

Crystal::Application* Crystal::CreateApplication() {
	return new Sandbox();
}