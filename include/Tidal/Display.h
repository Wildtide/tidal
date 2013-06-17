#pragma once

namespace Tidal
{
	class Application;

	class Display
	{
	private:
		int displayWidth;
		int displayHeight;
		bool oglInitialised;

		sf::RenderWindow* renderWindow;

		// SDL_Surface* sdlDisplaySurface;
		string windowTitle;

		Application* application;

		void InitOpenGL();
		void ShutdownOpenGL();

		bool ProcessEvents();
		void RenderFrame();

	public:
		Display(Application* application, int width, int height);
		~Display();

		void SetWindowTitle(string title);

		sf::RenderWindow* GetRenderWindow() const { return renderWindow; };

		void Startup();
		void Execute();
		void Shutdown();
	};

};