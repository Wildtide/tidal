#include <Tidal/tidal.h>

namespace Tidal
{

	Application::Application(ApplicationConfig &config)
	{
		configuration = config;
	}


	Application::~Application()
	{
	}


	void Application::Startup()
	{
		display = new Tidal::Display(this, configuration.displayWidth, configuration.displayHeight);
		display->Startup();
		display->SetWindowTitle(configuration.title);

		for (auto iter = services.begin(); iter != services.end(); ++iter)
		{
			(*iter)->Startup();
		}
	}


	void Application::Shutdown()
	{
		for (auto iter = services.rbegin(); iter != services.rend(); ++iter)
		{
			(*iter)->Shutdown();
		}

		display->Shutdown();
	}


	void Application::Execute()
	{
		display->Execute();
	}


	void Application::TriggerMessage(MessageId msgId)
	{
		Message msg(msgId);

		for (auto iter = services.begin(); iter != services.end(); ++iter)
		{
			(*iter)->OnMessage(&msg);
		}
	}


	int Application::GetDisplayWidth() const
	{
		return configuration.displayWidth;
	}


	int Application::GetDisplayHeight() const
	{
		return configuration.displayHeight;
	}


	sf::RenderWindow* Application::GetRenderWindow() const
	{
		return display->GetRenderWindow();
	}


	void Application::_AttachService(ApplicationService* svc)
	{
		services.push_back(svc);
		cout << "Attached: " << svc->GetServiceId() << endl;
	}

	ApplicationService* Application::GetService(string serviceId) const
	{
		for (auto iter = services.begin(); iter != services.end(); ++iter)
		{
			if (serviceId.compare((*iter)->GetServiceId()) == 0) return *iter;
		}
		return NULL;
	}


	void Application::SetMousePosition(int x, int y)
	{
		FramesService* svc = dynamic_cast<FramesService*>(GetService("FRAMES"));
		if (svc != NULL)
		{
			svc->SetMousePosition(x, y);
		}
	}

	void Application::SetMouseButtonDown(int button)
	{
		FramesService* svc = dynamic_cast<FramesService*>(GetService("FRAMES"));
		if (svc != NULL)
		{
			svc->SetMouseButtonDown(button);
		}
	}

	void Application::SetMouseButtonUp(int button)
	{
		FramesService* svc = dynamic_cast<FramesService*>(GetService("FRAMES"));
		if (svc != NULL)
		{
			svc->SetMouseButtonUp(button);
		}
	}

};
