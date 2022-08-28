#include "clpch.h"
#include "WindowsInput.h"
#include "Crystal/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Crystal {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return std::pair<float, float>((float)xpos, (float)ypos);
	}

	float WindowsInput::GetMouseXImpl() {
		return GetMousePositionImpl().first;
	}

	float WindowsInput::GetMouseYImpl() {
		return GetMousePositionImpl().second;
	}
}