#pragma once
namespace Crystal {
	class __declspec(dllexport) Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	};

	//To be defined in client
	Application* CreateApplication();
}
