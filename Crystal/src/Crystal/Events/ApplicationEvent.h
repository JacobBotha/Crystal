#pragma once

#include "Event.h"

namespace Crystal {
	class CRYSTAL_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(float width, float height)
			: m_Width(width), m_Height(height) {}
		
		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryApplication)
		EVENT_CLASS_TYPE(WindowResize)
	private:
		float m_Width, m_Height;
	};

	class CRYSTAL_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_CATEGORY(EventCategoryApplication)
		EVENT_CLASS_TYPE(WindowClose)
	};
}