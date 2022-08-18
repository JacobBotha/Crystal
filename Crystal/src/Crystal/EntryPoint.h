#pragma once

#ifdef CL_PLATFORM_WINDOWS

extern Crystal::Application* Crystal::CreateApplication();

int main(int argc, char** argv) {
	auto app = Crystal::CreateApplication();
	app->run();
	delete app;
}

#endif