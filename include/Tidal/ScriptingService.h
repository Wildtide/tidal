#pragma once

namespace Tidal
{

	class ScriptingService : public ApplicationService
	{
	private:
		lua_State* L;

	public:
		ScriptingService(Application* application);

		virtual void Startup();
		virtual void Shutdown();

		virtual void OnMessage(Message* msg);

		void RunScript(string script);
		void RunFile(string filename);
		lua_State* GetLuaState();
	};

}
