#include <Tidal/tidal.h>

using namespace Tidal;

int main(int argc, char* argv[])
{
	Application* app = new Application(ApplicationConfig(800, 600, "Test Harness"));

	try
	{
		app->AttachService<ScriptingService>();
		app->AttachService<FramesService>();

		app->Startup();

		ScriptingService* scripting = dynamic_cast<ScriptingService*>(app->GetService("SCRIPTING"));
		scripting->RunFile("./boot.lua");

		app->Execute();
		app->Shutdown();
	}
	catch (exception ex)
	{
		cerr << "Exception occurred: " << ex.what() << endl;
	}

	delete app;

    return 0;
}


// Keep windows happy...
int wmain(int argc, char* argv[]) { main(argc, argv); }
