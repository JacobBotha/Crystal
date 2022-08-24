#pragma once

#ifdef CL_PLATFORM_WINDOWS

extern Crystal::Application* Crystal::CreateApplication();

int main(int argc, char** argv) {
	Crystal::Log::Init();
	CL_CORE_WARN("Initialise Logger!");
	CL_INFO("Hello!");

	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}

#endif