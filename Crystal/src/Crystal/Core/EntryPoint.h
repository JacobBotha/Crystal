#pragma once

#ifdef CL_PLATFORM_WINDOWS

extern Crystal::Application* Crystal::CreateApplication();

int main(int argc, char** argv) {
	Crystal::Log::Init();
	CL_CORE_INFO("Initialise Logger!");

	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}

#endif