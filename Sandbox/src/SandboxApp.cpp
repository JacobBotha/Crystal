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
		if (Crystal::Input::IsKeyPressed(CL_KEY_TAB))
			CL_TRACE("Tab key is pressed!");
	}

	void OnEvent(Crystal::Event& event) override
	{
		CL_TRACE("{0}", event);
	}

	void OnAttach() override {
		m_VertShader = Crystal::Renderer::CreateShader("assets/shaders/shader.vert");
		m_FragShader = Crystal::Renderer::CreateShader("assets/shaders/shader.frag");
		
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