#pragma once
//------------------------------------------------------------------------------
/**
	Space game application

	(C) 20222 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include <render/server.h>
#include <render/client.h>

namespace Game
{
class SpaceGameApp : public Core::App
{
public:
	Server server;
	

	//void SpawnShipAtPosition(const glm::vec3& pos);
	//Client client;

	/// constructor
	SpaceGameApp();
	/// destructor
	~SpaceGameApp();

	/// open app
	bool Open();
	/// run app
	void Run();
	/// exit app
	void Exit();
private:

	/// show some ui things
	void RenderUI();

	Display::Window* window;
};
} // namespace Game