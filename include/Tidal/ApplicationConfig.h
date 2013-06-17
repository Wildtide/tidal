#pragma once

namespace Tidal
{
	class ApplicationConfig
	{
	public:

		ApplicationConfig() : displayWidth(640), displayHeight(480), title("Tidal Application")
		{
		}

		ApplicationConfig(int width, int height, string _title) : 
			displayWidth(width),
			displayHeight(height),
			title(_title)
		{
		}

		int displayWidth;
		int displayHeight;
		string title;
	};
};
