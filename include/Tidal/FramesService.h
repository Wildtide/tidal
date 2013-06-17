#pragma once

namespace Tidal
{

	class FramesService : public ApplicationService
	{
	private:
		ScriptingService* svcScripting;
		Frames::Environment* envFrames;

		void OnDraw();

	public:
		FramesService(Application* application);

		virtual void Startup();
		virtual void Shutdown();
		virtual void OnMessage(Message* msg);

		void SetMousePosition(int x, int y);
		void SetMouseButtonDown(int button);
		void SetMouseButtonUp(int button);

		void OnTestRawRender(Frames::EventHandle* event);
	};

}
