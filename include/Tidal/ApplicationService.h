#pragma once

namespace Tidal
{
	class Application;

	class ApplicationService
	{
	protected:
		Application* attachedToApp;
		string serviceId;

	public:
		ApplicationService(string _serviceId, Application* application) 
			: attachedToApp(application), serviceId(_serviceId)
		{}

		string GetServiceId() { return serviceId; }

		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		virtual void OnMessage(Message* msg) {};
	};

}
