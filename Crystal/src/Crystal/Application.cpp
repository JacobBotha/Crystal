#include "Application.h"
#include <stdio.h>

namespace Crystal {
	Application::Application() {}
	Application::~Application() {}
	void Application::run() {
		printf("Welcome to Crystal Engine!");
		while (true);
	}
}