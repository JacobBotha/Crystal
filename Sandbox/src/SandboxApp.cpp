#include <Crystal.h>
#include <Crystal/Renderer/Buffer.h>
#include <Crystal/Renderer/Vertex.h>

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

				CL_INFO("Tab key pressed - Reloading shaders!");
			}
		}

		if (event.GetEventType() == Crystal::EventType::KeyPressed) {
			//Convert event to key event
			Crystal::KeyPressedEvent* kEvent = (Crystal::KeyPressedEvent*)&event;
			//Check if Tab key is pressed and reload shaders/graphics pipeline
			if (kEvent->GetKeyCode() == Crystal::Key::Space) {
				m_TriangleVertices = {
					{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
					{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
					{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
				};
				m_TriangleVertexBuffer->BindData(m_TriangleVertices);
				Crystal::Renderer::Submit(m_TriangleVertexBuffer, m_TriangleVertices.size());
				CL_INFO("Space key pressed - Updating vertices!");
			}
		}

		if (event.GetEventType() == Crystal::EventType::KeyPressed) {
			Crystal::KeyPressedEvent* kEvent = (Crystal::KeyPressedEvent*)&event;
			if (kEvent->GetKeyCode() == Crystal::Key::C) {
				Crystal::Renderer::Clear();
				CL_INFO("C Key pressed - Clearing renderer!");
			}
		}
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

		m_TriangleVertices = {
			{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		size_t size = sizeof(Crystal::Vertex) * m_TriangleVertices.size();
		m_TriangleVertexBuffer = Crystal::Buffer::Create(Crystal::Buffer::BufferType::Vertex, size);
		m_TriangleVertexBuffer->BindData(m_TriangleVertices);
		Crystal::Renderer::Submit(m_TriangleVertexBuffer, m_TriangleVertices.size());
		
	}

	void OnDetach() override {
		m_VertShader.reset();
		m_FragShader.reset();
		m_TriangleVertexBuffer.reset();
	}

	std::shared_ptr<Crystal::Shader> m_VertShader;
	std::shared_ptr<Crystal::Shader> m_FragShader;

	std::shared_ptr<Crystal::Buffer> m_TriangleVertexBuffer;
	std::vector<Crystal::Vertex> m_TriangleVertices;


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