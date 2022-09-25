#pragma once

#include "Crystal/Core/JobManager.h"

#ifdef CL_PLATFORM_WINDOWS

extern Crystal::Application* Crystal::CreateApplication();

int main(int argc, char** argv) {
	Crystal::Log::Init();
	CL_CORE_INFO("Initialise Logger!");
	Crystal::JobManager::Init();
	CL_CORE_INFO("Initialise Job Manager!");

	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}

#endif