/*
   Gavin Johnson
   200467038
   grjohns5
*/

#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <math.h>
#include <random>
#include <unordered_map>
#include <zmq.h>

#include "Engine/RenderWindow.hpp"
#include "Engine/Entity.hpp"
#include "Engine/Color.hpp"
#include "Engine/Physics.hpp"
#include "Engine/KeyBindings.hpp"
#include "Engine/Thread.hpp"
#include "Engine/Networking.hpp"
#include "Engine/Timeline.hpp"
#include "Game.hpp"

int main(int argv, char **args)
{
	// Main is used for both the server and client depending on the cmd line args
	if (strcmp(args[1], "Server") == 0) {
		try {
			// we are spinning a server if we run the game as a server
			Networking *server = new Networking(args[4], true);
			server->spinServer(server, args[2], args[3]);
		} catch (const std::exception &e) {
			std::cout << "SERVER EXCEPTIONS......." << e.what() << std::endl;
		}
		return 0;
	}

	// Initializes the program a client window
	if (SDL_Init(SDL_INIT_VIDEO) > 0){
		std::cout << "SDL Init failed SDL Error" << SDL_GetError() << std::endl;
	}

	// Creates instance of the game object
	Game* gameInstance = new Game();
	RenderWindow window("Gavin HW3 Game", Physics::WORLD_BOUND_X[1]/2.0f, Physics::WORLD_BOUND_Y[1]);
	
	// Prepares networking variables, rendered scene, and timelines
	gameInstance->prepareNetworkVariables(args[4], args[2], args[3]);
	gameInstance->prepareScene();
	gameInstance->initializeTimeline();

	// Main Game Loop
	//for (int i = 0; i < 100; i++) {
	while (true) {
		// Check if window is closed at the beginning of the loop.
		if (window.isClosed()) { SDL_Quit();}

		// Get Delta time to use throughout the program (for physics and network information)
		float deltaTime = gameInstance->gameDelta();

		// Add to total deltas, used for SECTION 3 EXPERIMENT
		gameInstance->aggregateDeltas(deltaTime);

		// Networking tasks, create network threads
		gameInstance->sendHelloToServer();
		gameInstance->spawnP2PClientThreads();

		// Polls for events, which means checking for a closed window or changing window scale
		// Merge threads and quit in the event of a closed window. 
		gameInstance->setScale(window);	
		if (window.isClosed()){ gameInstance->mergeP2PThreads(); SDL_Quit(); break; }

		// Process information from server
		if (deltaTime != 0){ gameInstance->processServerInformation(); }

		// Process information from P2P
		gameInstance->processPeerInformation();
		
		// Local entity management, gathering input information and applying physics
		std::vector<float> forceApplied = gameInstance->getInput(deltaTime); // note: in getInput actionToPhy is called which induces physics changes for every keyboard input
		gameInstance->applyPhysics(forceApplied, deltaTime);
		gameInstance->renderScene(window);

		// Send update to other clients
		gameInstance->publishMessage(); // in p2p to all clients

		// Merge all threads
		gameInstance->mergeP2PThreads();

		// Delay the loop, in accordance with frame rate
		gameInstance->waitForCompletion(); //simulated wait for slower clients
	}
	return 0;
}
