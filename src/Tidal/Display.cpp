#include <Tidal/tidal.h>

namespace Tidal
{

	Display::Display(Application* _application, int width, int height)
	{
		application = _application;

		displayWidth = width;
		displayHeight = height;

		oglInitialised = false;
	}


	Display::~Display()
	{
	}


	void Display::Startup()
	{
		InitOpenGL();
	}


	void Display::Shutdown()
	{
		if (oglInitialised)
		{
			ShutdownOpenGL();
		}
	}


	void Display::RenderFrame()
	{
		renderWindow->clear();
		/*
		sf::Font font;
		font.loadFromFile("DroidSans.ttf");
		sf::Text text("Hello", font);
		renderWindow->draw(text);
		*/
		// Draw Callback Fire Here
		application->TriggerMessage(MessageId::msg_FrameBegin);		
		application->TriggerMessage(MessageId::msg_FrameEnd);		

		renderWindow->display();
	}


	void Display::Execute()
	{
		while (renderWindow->isOpen())
		{
			// Process events
			sf::Event event;
			while (renderWindow->pollEvent(event))
			{
				// Close window : exit
				if (event.type == sf::Event::Closed) renderWindow->close();

				if (event.type == sf::Event::MouseMoved)
				{
					sf::Vector2u windowSize = renderWindow->getSize();
					sf::Vector2i mousePos = sf::Mouse::getPosition(*renderWindow);
					if (mousePos.x >= 0 && mousePos.x < windowSize.x && mousePos.y >= 0 && mousePos.y < windowSize.y)
					{
						application->SetMousePosition(mousePos.x, mousePos.y);
					}
				}

				if (event.type == sf::Event::MouseButtonPressed)
				{					
					if (event.mouseButton.button == sf::Mouse::Button::Left)
					{
						SetCapture(renderWindow->getSystemHandle());
						application->SetMouseButtonDown(0);
					}
				}

				if (event.type == sf::Event::MouseButtonReleased)
				{
					if (event.mouseButton.button == sf::Mouse::Button::Left)
					{
						ReleaseCapture();
						application->SetMouseButtonUp(0);
					}
				}

			}

			RenderFrame();
		}
	}


	void Display::SetWindowTitle(string title)
	{
		windowTitle = title;
		if (renderWindow != NULL) renderWindow->setTitle(title.c_str());
	}


	void Display::InitOpenGL()
	{
		renderWindow = new sf::RenderWindow(sf::VideoMode(displayWidth, displayHeight), "SFML window");
		oglInitialised = true;

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			throw exception("Failed to initialise GLEW");
		}
	}

	void Display::ShutdownOpenGL()
	{
		delete renderWindow;
		renderWindow = NULL;
		oglInitialised = false;
	}

};
