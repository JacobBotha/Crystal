#pragma once

#include "Crystal/Core/Layer.h"

namespace Crystal {
	class CRYSTAL_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnUpdate() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override {}

	private:
		float m_Time = 0.0f;
	};
}
