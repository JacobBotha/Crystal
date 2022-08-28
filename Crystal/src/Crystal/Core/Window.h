#pragma once

#include "Crystal/Events/Event.h"

namespace Crystal {
	struct WindowProperties {
		const std::string& Title;
		unsigned int Width;
		unsigned int Height;

		WindowProperties(const std::string& title = "Crystal Engine", 
						 unsigned int height = 1280, 
			             unsigned int width= 720)
			: Title(title), Height(height), Width(width) {};
	};

	class CRYSTAL_API Window {
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual void SetEventCallBack(const EventCallBackFn callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() = 0;

		virtual void* GetNativeWindow() const = 0;
		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}