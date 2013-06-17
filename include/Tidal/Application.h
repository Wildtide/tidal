#pragma once

namespace Tidal
{

	class Application
	{
	private:
		ApplicationConfig configuration;
		Display* display;

		void _AttachService(ApplicationService* service);

		vector<ApplicationService*> services;

	public:
		Application(ApplicationConfig &config);
		~Application();

		template<class T>
		void AttachService()
		{
			_AttachService(new T(this));
		}

		void Startup();
		void Execute();
		void Shutdown();

		int GetDisplayWidth() const;
		int GetDisplayHeight() const;

		sf::RenderWindow* GetRenderWindow() const;

		void SetMousePosition(int x, int y);
		void SetMouseButtonDown(int button);
		void SetMouseButtonUp(int button);

		void TriggerMessage(MessageId id);
		ApplicationService* GetService(string serviceId) const;
	};

};
