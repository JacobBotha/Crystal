#pragma once

#include "Crystal/Core/Core.h"
#include "Crystal/Events/Event.h"

namespace Crystal {
	class CRYSTAL_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event& e) = 0;

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}