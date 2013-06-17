#include <Tidal/Tidal.h>
#include <fstream>
#include <frames/Environment_register.h>

namespace Tidal
{

	FramesService::FramesService(Application* application) 
		: ApplicationService("FRAMES", application)
	{
		svcScripting = dynamic_cast<ScriptingService*>(application->GetService("SCRIPTING"));
		if (svcScripting == NULL) throw exception("Scripting service is not attached");
	}


	void FramesService::OnDraw()
	{
		envFrames->Render();
	}


	void FramesService::Startup()
	{
		lua_State* L = svcScripting->GetLuaState();

		envFrames = new Frames::Environment();
		envFrames->ResizeRoot(attachedToApp->GetDisplayWidth(), attachedToApp->GetDisplayHeight()); 
		envFrames->LuaRegister(L);
		envFrames->LuaRegisterFramesBuiltin(L);

		envFrames->RegisterLuaFrameCreation<RawTest>(L);

		cout << "Started Frames service." << endl;
	}

	void FramesService::OnMessage(Message* msg)
	{
		switch (msg->msgId)
		{
		case MessageId::msg_FrameEnd:
			OnDraw();
			break;
		}
	}

	void FramesService::Shutdown()
	{
		if (envFrames != NULL)
		{
			//envFrames->LuaUnregister(svcScripting->GetLuaState());
			//delete envFrames;
			//envFrames = NULL;
		}

		cout << "Stopped Frames service" << endl;
	}

	void FramesService::SetMousePosition(int x, int y)
	{
		envFrames->MouseMove(x, y);
	}

	void FramesService::SetMouseButtonDown(int button)
	{
		envFrames->MouseDown(button);
	}

	void FramesService::SetMouseButtonUp(int button)
	{
		envFrames->MouseUp(button);
	}

}