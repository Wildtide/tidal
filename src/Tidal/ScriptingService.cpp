#include <Tidal/Tidal.h>

namespace Tidal
{

	ScriptingService::ScriptingService(Application* application) 
		: ApplicationService("SCRIPTING", application)
	{
		L = NULL;
	}

	void ScriptingService::Startup()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
		cout << "Scripting service started up" << endl;
	}

	void ScriptingService::Shutdown()
	{
		lua_close(L);
		L = NULL;
		cout << "Scripting service shutdown" << endl;
	}

	void ScriptingService::OnMessage(Message* msg)
	{
		switch (msg->msgId)
		{
		case MessageId::msg_FrameBegin:
			RunScript("if OnFrameBegin then OnFrameBegin() end");
			break;
		}
	}


	void ScriptingService::RunScript(string script)
	{
		int rc = luaL_loadstring(L, script.c_str());
		if (rc == 0)
		{
			// Script has been pushed as a function on the top of the stack
			int callrc = lua_pcall(L, 0, 0, 0);
			if (callrc != 0)
			{
				// stack contains error message
				string errMessage = lua_tostring(L, 1);
				lua_pop(L, 1);
				cerr << "Error executing lua script: " << errMessage << endl;
			}
		}
		else
		{
			// Error message is at the top of the stack
			string errMessage = lua_tostring(L, 1);
			lua_pop(L, 1);
			cerr << "Error executing lua script: " << errMessage << endl;
		}
	}

	void ScriptingService::RunFile(string filename)
	{		
		int rc = luaL_dofile(L, filename.c_str());
		if (rc != 0)
		{
			// Error message is at the top of the stack
			string errMessage = lua_tostring(L, 1);
			lua_pop(L, 1);
			cerr << "Error executing lua file: " << errMessage << endl;
		}
	}

	lua_State* ScriptingService::GetLuaState()
	{
		return L;
	}


}