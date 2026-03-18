//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "spacegameapp.h"
#include <enet/enet.h>

int
main(int argc, const char** argv)
{
	Game::SpaceGameApp app;

	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializng ENet.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);


	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}