#include <windows.h>
#include <frames/environment.h>
#include <frames/raw.h>
#include <gl/gl.h>
#include <gl/glu.h>
// #include <SDL/SDL.h>

#include <SFML/Graphics.hpp>
/*
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
*/

#include <iostream>

#include <string>
#include <map>
#include <vector> 

#include <lua.hpp>

using namespace std;

#include <frames/delegate.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Display.h"
#include "ApplicationConfig.h"
#include "Message.h"
#include "ApplicationService.h"
#include "Application.h"

#include "ScriptingService.h"
#include "FramesService.h"

#include "RawTest.h"
