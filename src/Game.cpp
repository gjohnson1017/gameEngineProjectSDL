#include "Game.hpp"

#include <cmath>


struct Color bgColor{0,150,255,255};
struct Color playerColor{255,150,255,255};
struct Color groundColor{0,255,100,255};
struct Color enemyColor{200,0,0,255};
struct Color buddyColor{ 255, 0, 255, 255 };
struct Color platform_s1_Color { 0, 42, 200, 255 };
struct Color platform_s2_Color { 0, 42, 100, 230 };
struct Color platform_s3_Color { 0, 10, 50, 10 };
struct Color platform_d_Color { 0, 255, 42, 255 };
struct Color plateColor { 255, 255, 0, 255 };

float tic = 20.0;

float Physics::gravity = 0.08;
int Physics::WORLD_DIMENSIONS = 2;
const int DEFAULT_WIDTH = 1280;
std::vector<int> Physics::WORLD_BOUND_X = {0, 2 * DEFAULT_WIDTH};
std::vector<int> Physics::WORLD_BOUND_Y = {NULL, 720};
bool RenderWindow::propScale = false;

Game::Game(){	
	frameNumber = 0;
	totalDeltas=0.0f;
}

void Game::setScale(RenderWindow &window){
	scale = window.pollEvents(DEFAULT_WIDTH) / DEFAULT_WIDTH; 
}

/*////////////////////////////////////////////
  Key binding is done here. raw inputs are mapped to action which is then used by actionToPhy
*/////////////////////////////////////////////
KeyBindings* bindPlayerKeys()
{
	std::map<SDL_Scancode, Action> keybinds;
	keybinds[SDL_SCANCODE_UP] = MOVE_UP;
	keybinds[SDL_SCANCODE_LEFT] = MOVE_LEFT;
	keybinds[SDL_SCANCODE_RIGHT] = MOVE_RIGHT;
	keybinds[SDL_SCANCODE_DOWN] = MOVE_DOWN;

	KeyBindings *keybind = new KeyBindings();
	keybind->setKeyBinds(keybinds);
	return keybind;
}

/*////////////////////////////////////////////
  Prepare a scene by creating all entities and setting all the game variables required for the scene
*/////////////////////////////////////////////
void Game::prepareScene(){

    // Entity attributes initialization

	// special seperation for players
	float mass_player = 10;
    std::vector<float> d_player{30, 30};
    const std::vector<int> origin_player{100, 2 * Physics::WORLD_BOUND_Y[1]/3 - (int)d_player[1]};
    std::vector<int> c_player = origin_player;
	player = new Entity(PLAYER, c_player, d_player, mass_player, bindPlayerKeys());
	entityList.push_back(player);

    // Coordinates -- init
    std::vector<int> groundCoords{0, 2 * Physics::WORLD_BOUND_Y[1]/3};
    std::vector<int> platformCoords{500, Physics::WORLD_BOUND_Y[1] / 2 - 2 * (int)d_player[1]};
    std::vector<int> pressurePlateCoord{Physics::WORLD_BOUND_X[1] / 4 + 30, Physics::WORLD_BOUND_Y[1] / 2 + 40};

    // Dimension -- init
    std::vector<float> groundDimens{(float)Physics::WORLD_BOUND_X[1], Physics::WORLD_BOUND_Y[1]/3.0f};
    std::vector<float> platformDimens{100, 20};
    std::vector<float> pressurePlateDimen{50,  10 };

	// create all entities
	Entity *ground = new Entity(PLATFORM, groundCoords, groundDimens, 0);
	//Entity *staticPressurePlate = new Entity(NPC, pressurePlateCoord, pressurePlateDimen,0);

	// pressure plate
	
	// Static Platforms
	for(int i = 0; i< 5; i++) {
		std::vector<float> d_plat{125, 15};
		std::vector<int> c_plat{450 + (i * (100 + (int)d_plat[0])), 2 * Physics::WORLD_BOUND_Y[1]/3 - 8 * (int)d_plat[1]};

		std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
		Entity* platform = new Entity(PLATFORM_STATIC_1, c_plat, d_plat, 0);
		entityList.push_back(platform);
	}

	for(int i = 0; i< 3; i++) {
		std::vector<float> d_plat{125, 15};
		std::vector<int> c_plat{600 + (i * (100 + (int)d_plat[0])), 2 * Physics::WORLD_BOUND_Y[1]/3 - 16 * (int)d_plat[1]};

		std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
		Entity* platform = new Entity(PLATFORM_STATIC_2, c_plat, d_plat, 0);
		entityList.push_back(platform);
	}

	std::vector<float> d_plat{125, 15};
		std::vector<int> c_plat{700 + (100 + (int)d_plat[0]), 2 * Physics::WORLD_BOUND_Y[1]/3 - 24 * (int)d_plat[1]};

		std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
		Entity* platform = new Entity(PLATFORM_STATIC_3, c_plat, d_plat, 0);
		entityList.push_back(platform);

    // Push entities to the list
    
	entityList.push_back(ground);
	//entityList.push_back(staticPlat0);

	setSpawnZones();
	setDeadZones();

	plateOn = false; // check if pressue plate is on
	enemyHit = false; // check if game should be paused to simulate game over 	
}
/*////////////////////////////////////////////
  set all the timeline variables being used in the game loop
*/////////////////////////////////////////////
void Game::initializeTimeline(){
	globalTime = new Timeline(NULL, 1);
	globalTime->setParentTime();
	gameTime = new Timeline(globalTime, 10);
	player->setEntityTimeLine(gameTime);
	lastGameTime = gameTime->getTime();
	deltaGameAccumulator = 0;

}

/*////////////////////////////////////////////
  Prepare a scene by creating all entities and setting all the game variables required for the scene
*/////////////////////////////////////////////
float Game::gameDelta(){
	globalTime->setParentTime();
	currentGameTime = gameTime->getTime();

	float gameDeltaTime = currentGameTime - lastGameTime;

	if (gameDeltaTime < 0 || gameDeltaTime > 30)
		gameDeltaTime = deltaGameAccumulator;
	if (gameDeltaTime > 0)
		deltaGameAccumulator = (0.7 * gameDeltaTime) + (0.3 * deltaGameAccumulator);

	lastGameTime = currentGameTime;
	return gameDeltaTime;
}

/*////////////////////////////////////////////
	perform action on the player based on keyinput
*/////////////////////////////////////////////
std::vector<float> Game::getInput(float playerDelta){
	const Uint8 *keypressed = SDL_GetKeyboardState(NULL);

    // for all the movement keys, we perform actionToPhy. "action to physics"
	std::vector<float> forceApplied = {0.0f, 0.0f};
	if (keypressed[SDL_SCANCODE_UP] || keypressed[SDL_SCANCODE_DOWN] || keypressed[SDL_SCANCODE_LEFT] || keypressed[SDL_SCANCODE_RIGHT]){
		if (keypressed[SDL_SCANCODE_UP] && player->getEntityPhysics()->getAccelerationComponent(1) == 0.0f){
			forceApplied = player->actionToPhy(SDL_SCANCODE_UP, playerDelta, 9, forceApplied);
		}
		if (keypressed[SDL_SCANCODE_DOWN]){
			forceApplied = player->actionToPhy(SDL_SCANCODE_DOWN, playerDelta, .1, forceApplied);
		}
		if (keypressed[SDL_SCANCODE_LEFT]){
			forceApplied = player->actionToPhy(SDL_SCANCODE_LEFT, playerDelta, 50.0, forceApplied);
		}
		if (keypressed[SDL_SCANCODE_RIGHT]){
			forceApplied = player->actionToPhy(SDL_SCANCODE_RIGHT, playerDelta, 50.0, forceApplied);
		}
	} else if (keypressed[SDL_SCANCODE_0] && SDL_GetTicks() - togPressed > 500){ // toggles between constant and propotional scaling with 0
        // adds a cooldown period between toggles
		togPressed = SDL_GetTicks();
		RenderWindow::toggleScaling();
	}
    // pause game with P
	if (keypressed[SDL_SCANCODE_P] && SDL_GetTicks() - togPressed > 1000){
		togPressed = SDL_GetTicks();
		togglePause(player->getEntityTimeLine());
	}

    // slow motion game
	if (keypressed[SDL_SCANCODE_2]){
		tic = 50.0;
	} else if (keypressed[SDL_SCANCODE_1])	{ // Normal Game speed
		tic = 20.0;
	}
	player->getEntityTimeLine()->changeTic(tic);

	return forceApplied;

}

void Game::applyPhysics(std::vector<float> forceApplied, float deltaTime){
	int constantTime = 1;
	int amountOfSkips = (int)deltaTime / (int)constantTime == 0 ? 1 : deltaTime / constantTime;
	if(deltaTime < 1 && deltaTime > 0) constantTime = deltaTime;
	if(deltaTime == 0) return;

	int itr = 0;
	while(itr < amountOfSkips){
		player->getEntityPhysics()->changeState(forceApplied, 1, itr);
		collisionDetection(1);
		itr++;
	}

	player->getEntityPhysics()->setVelocity({0.0f, player->getEntityPhysics()->getVelocityComponent(1)});
	player->getEntityPhysics()->setAcceleration({0.0f, player->getEntityPhysics()->getAccelerationComponent(1)});

}


void Game::prepareNetworkVariables(std::string uniqueId, std::string port1Val, std::string port2Val){
	sendBufferMutex = new std::mutex();
	recvBufferMutex = new std::mutex();
	pubBufferMutex = new std::mutex();
	subBufferMutex = new std::mutex();

	sendBuffer = new std::string();
	recvBuffer = new std::string();
	pubBuffer = new std::string();
	subBuffer = new std::string();
	*subBuffer = "";

	clientUniqueId = std::string(uniqueId);

	serverPort1 = std::string(port1Val);
	serverPort2 = std::string(port2Val);

	clientPort1 = std::string("tcp://localhost:"+std::to_string((std::stoi(serverPort1.substr(serverPort1.length() - 4)) + 2 * std::stoi(clientUniqueId))));
	clientPort2 = std::string("tcp://localhost:"+std::to_string((std::stoi(serverPort2.substr(serverPort2.length() - 4)) + 2 * std::stoi(clientUniqueId))));

	std::string::iterator end_pos = std::remove(clientUniqueId.begin(), clientUniqueId.end(), ' ');
	clientUniqueId.erase(end_pos, clientUniqueId.end());

	client = new Networking(clientUniqueId, false);
	context = zmq::context_t(1);
	client->connect(client, serverPort1, serverPort2, &context, "(" + clientUniqueId + ")[" + clientPort1 + "," + clientPort2 + "]", sendBuffer, recvBuffer, sendBufferMutex, recvBufferMutex);

	pub_socket = new zmq::socket_t(context, zmq::socket_type::pub);
	pub_socket->bind(clientPort2);
}

// networking wrapper functions to help with threading
// wrapper for client's publishing to other p2p clients
void clientPubWrapper(Networking *client, std::mutex *pubBufferMutex, std::string *pubBuffer, zmq::socket_t* pub_socket) {
	{	client->pubClientFunction(pubBufferMutex, pubBuffer, pub_socket); }
}

// wrapper for client subscribed to other p2p clients and the server
void clientSubWrapper(Networking *client, std::mutex *subBufferMutex, std::string *subBuffer, std::string port) {
	{	client->subClientFunction(subBufferMutex, subBuffer, port);}
}

// wrapper for clients recieving messages from the server
void clientRecvWrapper(Networking *client, std::mutex *recvBufferMutex, std::string *recvBuffer) {
	{	client->recvClientFunction(recvBufferMutex, recvBuffer); }
}

void Game::spawnP2PClientThreads(){
	// spawn the client's thread that recieves messages from server
	clientRecvSpawnThread = std::thread(clientRecvWrapper, client, recvBufferMutex, recvBuffer);
	// spawn the client's thread that publishes messages to other clients
	clientPubSpawnThread = std::thread(clientPubWrapper, client, pubBufferMutex, pubBuffer, pub_socket);
		
	for (auto const &x : client_clientVsPort)
	{

		if (client->clientVsSocket.count(client_clientVsPort[x.first][1]) != 1)
		{
			client->createSocket(&context, client_clientVsPort[x.first][1]);
		}

		// spawn all the threads that are subscribers to other clients
		clientSubThreads.push_back(std::thread(clientSubWrapper, client, subBufferMutex, subBuffer, client_clientVsPort[x.first][1]));
	
	}
}

// Helper method called by process server information to read message
void parseServerMessage(std::vector<std::string> &vectorOfPorts, std::mutex* recvBufferMutex, std::string* recvBuffer){
	{
		std::lock_guard<std::mutex> lock(*recvBufferMutex);
		//std::cout<<"Message read from server : "<<*recvBuffer<<std::endl;
		while (true){
			if (recvBuffer->find(':') != std::string::npos){
				std::string portString = recvBuffer->substr(0, recvBuffer->find(';'));
				vectorOfPorts.push_back(portString);
				recvBuffer->erase(0, recvBuffer->find(';') + 1);
			} else {
				break;
			}
		}
	}
}

// Process moving objects
void Game::processServerInformation(){
	std::vector<std::string> vectorOfPorts;
	
	parseServerMessage(vectorOfPorts, recvBufferMutex, recvBuffer); // parse and store all the messages in vectorOfPorts
	std::unordered_set<std::string> newActiveClients;

	for (int i = 0; i < vectorOfPorts.size(); i++){
		std::string portString = vectorOfPorts[i];

		// Extract entityId aactionToPhynd entityProps
		std::string clientId = portString.substr(0, portString.find(':'));
		std::string portDets = portString.substr(portString.find(':') + 1);

		newActiveClients.insert(clientId);

		// Extract xcoord and ycoord
		size_t x_start = portString.find('[') + 1;
		size_t x_end = portString.find(',');

		size_t y_start = x_end + 1;
		size_t y_end = portString.find(']');

		std::string subPort = portString.substr(x_start, x_end - x_start); // this is the subPort or xCoord for all server controlled entities
		std::string pubPort = portString.substr(y_start, y_end - y_start); // this is the pubPort or yCoord for all server controlled entities

		if (clientId.compare(clientUniqueId) != 0 && entityIdVsProp.count(clientId) != 1)
		{
			
			if (clientId.find("PLATFORM") != std::string::npos || clientId.find("ENEMY") != std::string::npos)
			{   // Convert xcoord and ycoord to float
				float xcoord = std::stof(subPort);
				float ycoord = std::stof(pubPort);

				std::vector<int> newLocation = {int(xcoord), int(ycoord)};
				if(clientId.find("PLATFORM") != std::string::npos)
					entityIdVsProp[clientId] = new Entity(PLATFORM, std::vector<int>{int(xcoord), int(ycoord)}, {125, 15}, {0.0f, 0.0f}, {0.0f, 0.0f} , {0.0f, 0.0f});
				else
					entityIdVsProp[clientId] = new Entity(ENEMY, std::vector<int>{int(xcoord), int(ycoord)}, {25, 25}, {0.0f, 0.0f}, {0.0f, 0.0f} , {0.0f, 0.0f});
				
				entityList.push_back(entityIdVsProp[clientId]);
				//entityIdVsIndex[clientId] = entityList.size() - 1;
			}
			else {
				//std::cout<<" CLIENT VS PORT : "<<clientId<<" "<< subPort<<" "<<pubPort<<std::endl;
				std::vector<std::string> ports{subPort, pubPort};
				client_clientVsPort[clientId] = ports;
			}
		} else if (clientId.compare(clientUniqueId) != 0 && (clientId.find("PLATFORM") != std::string::npos || clientId.find("ENEMY") != std::string::npos)) {
			float xcoord = std::stof(subPort);
			float ycoord = std::stof(pubPort);

			std::vector<int> newLocation = {int(xcoord), int(ycoord)};
			entityIdVsProp[clientId]->setLocation(new Coordinates(newLocation));

		}
	}

	

	if( vectorOfPorts.size()) {
		for (const std::string &item : activeClients) {
			if(entityIdVsProp.count(item)!=0 
				&& entityIdVsProp[item]->getType() == BUDDIES 
				&& newActiveClients.count(item) == 0) {
				int index = 0;
				for(int i=0;i<entityList.size();i++){
					if(entityIdVsProp[item] == entityList[i]){
						index = i;
					}
				}
				
				entityList.erase(entityList.begin() + index);
				entityIdVsProp[item] = nullptr;
			}
		}
		activeClients = newActiveClients;
	}
}

// Process other client positions (P2P)
void Game::processPeerInformation(){
	if(subBuffer->compare("") != 0){
		std::vector<std::string> vectorOfBuddies;
		std::lock_guard<std::mutex> lock(*subBufferMutex);
		while (true){
			if (subBuffer->find(':') != std::string::npos){
				std::string portString = subBuffer->substr(0, subBuffer->find(';'));
				vectorOfBuddies.push_back(portString);
				subBuffer->erase(0, subBuffer->find(';') + 1);
			}else{
				break;
			}
		}

		for (int i = 0; i < vectorOfBuddies.size(); i++) {
			std::string buddy = vectorOfBuddies[i];

			// Extract entityId and entityProps
			std::string buddyId = buddy.substr(buddy.find('}')+1, buddy.find(':') - buddy.find('}') - 1);
			std::string buddyDets = buddy.substr(buddy.find(':') + 1);

			// Extract xcoord and ycoord
			size_t x_start = buddy.find('<') + 1;
			size_t x_end = buddy.find(',');

			size_t y_start = x_end + 1;
			size_t y_end = buddy.find('>');

			// Extract xcoord and ycoord as strings
			std::string x_coord = buddy.substr(x_start, x_end - x_start);
			std::string y_coord = buddy.substr(y_start, y_end - y_start);

			if (buddyId.compare(clientUniqueId) != 0 && entityIdVsProp.count(buddyId) != 1)
			{

				float xcoord = std::stof(x_coord);
				float ycoord = std::stof(y_coord);

				std::vector<int> newLocation = {int(xcoord), int(ycoord)};
				entityIdVsProp[buddyId] = new Entity(BUDDIES, std::vector<int>{int(xcoord), int(ycoord)}, {player->getDimensionW(), player->getDimensionH()}, 10);
				entityList.push_back(entityIdVsProp[buddyId]);
				//entityIdVsIndex[buddyId]= entityList.size() - 1;
			} else if (buddyId.compare(clientUniqueId) != 0) {

				float xcoord = std::stof(x_coord);
				float ycoord = std::stof(y_coord);

				std::vector<int> newLocation = {int(xcoord), int(ycoord)};
				entityIdVsProp[buddyId]->setLocation(new Coordinates(newLocation));
			}
		}

		*subBuffer = "";
	}
}

// Render each enumeration with the adjusted factor for side scrolling
void Game::renderScene(RenderWindow &window){
	window.renderClear(bgColor); // set background color
	float adjustFactor = 0.0f;
	
	if(player->getLocation()[0] > (float)window.getWidth() * 3.0f/4.0f){
		adjustFactor = player->getLocation()[0] - (float)window.getWidth() * 3.0f/4.0f;
	}

	for (int i = 0; i < entityList.size(); i++){
		if(entityList[i]->getType() == PLAYER) window.renderEntity(entityList[i], playerColor, scale, adjustFactor);
		else if (entityList[i]->getType() == GROUND) window.renderEntity(entityList[i], groundColor, scale, adjustFactor);
		else if (entityList[i]->getType() == PLATFORM) window.renderEntity(entityList[i], platform_d_Color, scale, adjustFactor);
		else if (entityList[i]->getType() == PLATFORM_STATIC_1) window.renderEntity(entityList[i], platform_s1_Color, scale, adjustFactor);
		else if (entityList[i]->getType() == PLATFORM_STATIC_2) window.renderEntity(entityList[i], platform_s2_Color, scale, adjustFactor);
		else if (entityList[i]->getType() == PLATFORM_STATIC_3) window.renderEntity(entityList[i], platform_s3_Color, scale, adjustFactor);
		else if (entityList[i]->getType() == BUDDIES) window.renderEntity(entityList[i], buddyColor, scale, adjustFactor);
		else if (entityList[i]->getType() == ENEMY) window.renderEntity(entityList[i], enemyColor, scale, adjustFactor);
	}

	window.renderScene();
}

/*////////////////////////////////////////////
	collision detection
*/////////////////////////////////////////////
void Game::collisionDetection(float deltaTime){
	for(int i = 0; i<entityList.size();i++){
		if(entityList[i]->getType() != BUDDIES) continue;
		std::map<Entity *, std::string> newDetectionMap = Entity::detectCollision(entityList[i], entityList, deltaTime);
		std::map<Entity *, std::string>::iterator newItr;

		for (newItr = newDetectionMap.begin(); newItr != newDetectionMap.end(); ++newItr){
			if (newItr->first == NULL) continue;
			if (newItr->first->getType() == NPC){
				plateOn = true;
			}
		}
	}

	std::map<Entity *, std::string> detectionMap = Entity::detectCollision(player, entityList, deltaTime);
	std::map<Entity *, std::string>::iterator itr;

	for (itr = detectionMap.begin(); itr != detectionMap.end(); ++itr){
		
		if (itr->first == NULL) continue;
		// apply changes to player entity if the second entity is a platform or buddy
		if (itr->first->getType() == PLATFORM || itr->first->getType() == BUDDIES || itr->first->getType() == GROUND || itr->first->getType() == PLATFORM_STATIC_1 && !plateOn 
		|| itr->first->getType() == PLATFORM_STATIC_2 && !plateOn || itr->first->getType() == PLATFORM_STATIC_3 && !plateOn){
			// this is game specific put you platform collision logic here
			if (itr->second == "TOP"){
				if ((player->getLocation()[1] + (int)player->getDimensionH() - itr->first->getLocation()[1] < player->getDimensionH() / 2 && player->getEntityPhysics()->getVelocityComponent(1) >= 0.0f)) {
					//  the player is then moved to stand exactly above the platform's x coordinate to help against cases where the player phases through the platform despite collision
					std::vector<int> newCoords = {player->getLocation()[0], itr->first->getLocation()[1] - (int)player->getDimensionH()};
					player->setLocation(new Coordinates(newCoords));

					// negate the y velocity and accelaration to place the player right on top of the platform.
					player->getEntityPhysics()->setVelocity({player->getEntityPhysics()->getVelocityComponent(0), 0.0f});
					player->getEntityPhysics()->setAcceleration({player->getEntityPhysics()->getAccelerationComponent(0), 0.0f});
				}
			}else if (itr->second == "BOTTOM"){
				// std::vector<int> newCoords = {player->getLocation()[0], itr->first->getLocation()[1] + (int)itr->first->getDimensionH()};
				// player->setLocation(new Coordinates(newCoords));
				if ((player->getLocation()[1] + (int)player->getDimensionH() > itr->first->getLocation()[1])) {
					player->getEntityPhysics()->setVelocity({player->getEntityPhysics()->getVelocityComponent(0), 0.0f});
					//player->getEntityPhysics()->setAcceleration({player->getEntityPhysics()->getAccelerationComponent(0),0.0f});
				}
			}
		}
		if (itr->first->getType() == ENEMY || itr->first->getType() == ZONE){
			reSpawn();
			break;
		}
		if (itr->first->getType() == NPC){
			plateOn = true;
		}
		if(itr->first->getType() == PLATFORM_STATIC_1 || itr->first->getType() == PLATFORM_STATIC_2 || itr->first->getType() == PLATFORM_STATIC_3){
			if (itr->second == "LEFT"){
				std::vector<int> newCoords = {itr->first->getLocation()[0] - 50, player->getLocation()[1]};
				player->setLocation(new Coordinates(newCoords));
				player->getEntityPhysics()->setVelocity({0.0f, player->getEntityPhysics()->getVelocityComponent(1)});
				//player->getEntityPhysics()->setAcceleration({0.0f, player->getEntityPhysics()->getAccelerationComponent(1)});
			}
			else if (itr->second == "RIGHT"){
				std::vector<int> newCoords = {itr->first->getLocation()[0] + (int)itr->first->getDimensionW(), player->getLocation()[1]};
				player->setLocation(new Coordinates(newCoords));
				player->getEntityPhysics()->setVelocity({0.0f, player->getEntityPhysics()->getVelocityComponent(0)});
				//player->getEntityPhysics()->setAcceleration( { 0.0f, player->getEntityPhysics()->getAccelerationComponent(0)});
			}	
		}
	}
}

void Game::publishMessage(){
	{
		std::lock_guard<std::mutex> lock(*pubBufferMutex);
		*pubBuffer = "";
		*pubBuffer += "(" + clientUniqueId + "){" + std::to_string(globalTime->getTime()) + "}" + clientUniqueId + ":<" + std::to_string(player->getLocation()[0]) + "," + std::to_string(player->getLocation()[1]) + ">;";
	}
}

// Merge threads
void Game::mergeP2PThreads(){
	if(clientRecvSpawnThread.joinable()) clientRecvSpawnThread.join();
	if(clientPubSpawnThread.joinable()) clientPubSpawnThread.join();
	for (int i = 0; i < clientSubThreads.size(); i++) {
		if(clientSubThreads[i].joinable()) clientSubThreads[i].join();
	}
}

// Set zones for players to spawn when they are eliminated
void Game::setSpawnZones(){
	spawnZones.push_back({100, 2 * Physics::WORLD_BOUND_Y[1]/3 - 30});
	spawnZones.push_back({1280, 2 * Physics::WORLD_BOUND_Y[1]/3 - 30});
}	

// Set boundaries that reset player position after crossing them
void Game::setDeadZones(){
	std::vector<int> deadZoneCoord{Physics::WORLD_BOUND_X[0], Physics::WORLD_BOUND_Y[0]};
	std::vector<float> deadZoneDimens{5, (float)Physics::WORLD_BOUND_Y[1]};
	Entity* deadZone = new Entity(ZONE, deadZoneCoord, deadZoneDimens, 0);
	entityList.push_back(deadZone);

	std::vector<int> deadZoneCoord1{Physics::WORLD_BOUND_X[1], Physics::WORLD_BOUND_Y[0]};
	std::vector<float> deadZoneDimens1{5, (float)Physics::WORLD_BOUND_Y[1]};
	Entity* deadZone1 = new Entity(ZONE, deadZoneCoord1, deadZoneDimens1, 0);
	entityList.push_back(deadZone1);
}

void Game::reSpawn(){
	int minDistance = INT_MAX;
	std::vector<int> bestSpawnZone = spawnZones[0];
	for(int i=0;i<spawnZones.size();i++){
		int dist = abs(player->getLocation()[0]-spawnZones[i][0]);
		if(minDistance > dist){
			bestSpawnZone = spawnZones[i];
			minDistance = dist;
		}
	}
	std::vector<int> newCoords = {bestSpawnZone[0], bestSpawnZone[1]};
	player->setLocation(new Coordinates(newCoords));
}

// periodically send message to server saying you are alive
void Game::sendHelloToServer() {
	if(frameNumber % 5 == 0) client->sendToServer( "(" + clientUniqueId + ")[" + clientPort1 + "," + clientPort2 + "]" );
	frameNumber+=1;
	// if(totalDeltas >= 10000) {
	// 	std::cout<< "TIME TAKEN FOR THE EXPERIMENT : "<<globalTime->getTime() - globalTime->getStartTime()<<std::endl;
	// 	exit(0);	
	// }
}

// Cumulative delta used for testing
void Game::aggregateDeltas(float delta){
	totalDeltas += delta < 1 ? 1 : delta; 
}

// Delegates frame rate
void Game::waitForCompletion(){
	float deltaTime = gameDelta();
	//std::cout<<"WAIT FOR : "<<(1-deltaTime) * gameTime->getTics();
	if(deltaTime < 1 && deltaTime >= 0) SDL_Delay((1-deltaTime) * gameTime->getTics());
}

// Closes threads if loop ends or window is shut
void Game::closeAllThreads(){
	mergeP2PThreads();
	clientRecvSpawnThread.std::thread::~thread();
	clientPubSpawnThread.std::thread::~thread();
	for (int i = 0; i < clientSubThreads.size(); i++) {
		clientSubThreads[i].std::thread::~thread();
	}
	context.close();
	//exit(0);
	std::cout<<"game destroyed "<<std::endl;
}

Game::~Game(){
	//closeAllThreads();
	std::cout<<"game destroyed "<<std::endl;
}
