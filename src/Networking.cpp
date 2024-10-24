#include "Engine/Networking.hpp"
#include <stdexcept>
#include <zmq.hpp>
#include <thread>
#include <unistd.h>
#include <queue>

// SERVER - Queue that contains all client entity information. 
// Information is queued and then popped so it can be published to all clients later. 
// Used in serverRead - server accepts client information and adds to the queue
// Used in serverPublish - server clears queue and processes client input and sends out to clients
std::queue<std::string> serverQueue;

// SERVER - Helper to erase leftover information in string. 
std::string eraseSpace(std::string oldstr){
	std::string str(oldstr);
	std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
	str.erase(end_pos, str.end());
	return str;
}

// SERVER - Creates the main networking object for servers
// Creates moving objects such as enemies and platforms
Networking::Networking(std::string p_uniqueId, bool p_isServer): uniqueId(p_uniqueId), isServer(p_isServer) {
	if(isServer){
		// Sets initial clients to 0
		nuOfClients = 0;
		
		std::vector<float> enemy_patternLimitsY{0.0f, 0.0f};
		std::vector<float> enemy_patternVelocity{300.0f, 0};
		std::vector<float> enemy_patternVelocityAlt{400.0f, 0};


		// entity creation 
		for(int i = 0; i< 4; i++) {
			std::vector<float> d_enemy{125, 25};
			std::vector<int> c_enemy1{150, 2 * Physics::WORLD_BOUND_Y[1]/3 - 4 * (int)d_enemy[1] - 75 * (i)};
			std::vector<int> c_enemy2{500, 2 * Physics::WORLD_BOUND_Y[1]/3 - 4 * (int)d_enemy[1] - 75 * (i)};

			std::vector<float> enemy_patternLimitsX{0, 1280};
			if (i % 2 == 0) {
				Entity* enemy = new Entity(ENEMY, c_enemy1, d_enemy, enemy_patternVelocity, enemy_patternLimitsX, enemy_patternLimitsY);
				entityList.push_back(enemy);
			} else {
				Entity* enemy = new Entity(ENEMY, c_enemy2, d_enemy, enemy_patternVelocityAlt, enemy_patternLimitsX, enemy_patternLimitsY);
				entityList.push_back(enemy);
			}
		}

		//entity pattern
		
		std::vector<float> platform_patternLimitsY{0.0f, 0.0f};
		std::vector<float> platform_patternVelocity{50.0f, 0};

		std::vector<float> platform_patternVelocityAlt{-50.0f, 0};


		// entity creation 
		for(int i = 0; i< 6; i++) {
			std::vector<float> d_plat{125, 15};
			std::vector<int> c_plat{350 + (i * (100 + (int)d_plat[0])), 2 * Physics::WORLD_BOUND_Y[1]/3 - 4 * (int)d_plat[1]};

			std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
			Entity* platform = new Entity(PLATFORM, c_plat, d_plat, platform_patternVelocity, platform_patternLimitsX, platform_patternLimitsY);
			entityList.push_back(platform);
		}

		for(int i = 0; i< 4; i++) {
			std::vector<float> d_plat{125, 15};
			std::vector<int> c_plat{475 + (i * (100 + (int)d_plat[0])), 2 * Physics::WORLD_BOUND_Y[1]/3 - 12 * (int)d_plat[1]};

			std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
			Entity* platform = new Entity(PLATFORM, c_plat, d_plat, platform_patternVelocityAlt, platform_patternLimitsX, platform_patternLimitsY);
			entityList.push_back(platform);
		}

		for(int i = 0; i< 2; i++) {
			std::vector<float> d_plat{125, 15};
			std::vector<int> c_plat{600 + (i * (100 + (int)d_plat[0])), 2 * Physics::WORLD_BOUND_Y[1]/3 - 20 * (int)d_plat[1]};

			std::vector<float> platform_patternLimitsX{d_plat[0] - 50.0f, d_plat[0] + 50.0f};
			Entity* platform = new Entity(PLATFORM, c_plat, d_plat, platform_patternVelocity, platform_patternLimitsX, platform_patternLimitsY);
			entityList.push_back(platform);
		}

		// push entity to list
		
	}
}



// SERVER - Wrapper for pull_socket
void serverReadWrapper(Networking* server,std::mutex* serverQueueMutex){
	server->serverRead(serverQueueMutex);
}

// Wrapper for pub_socket
void serverPubWrapper(Networking* server,std::mutex* serverQueueMutex, std::mutex* entityMutex){
	server->serverPublish(serverQueueMutex, entityMutex);
}

// SERVER - Executes patterns, used for platforms AND obstacles/enemies
void movingPlatform(std::vector<Entity*> entityList, float entityDelta){
	for(int i=0;i<entityList.size();i++) {
		if(entityList[i]->getType() == PLAYER) continue;
		if(entityDelta > 0.0 && entityList[i]->getType() != PLAYER && entityList[i] != NULL && (entityList[i]->getEntityPhysics()->getVelocityComponent(0) != 0 || entityList[i]->getEntityPhysics()->getVelocityComponent(1) != 0)){
			entityList[i]->getEntityPhysics()->executePattern(entityDelta);
		}
	}
}

// SERVER - Reads information from the client and stores it in a queue
void Networking::serverRead(std::mutex* serverQueueMutex){
	try{
		zmq::message_t request;
		while(pull_socket->recv(request, zmq::recv_flags::dontwait)){
			std::string req_str(static_cast<char*>(request.data()), request.size());
			std::cout<<"SERVER READS : "<<req_str<<std::endl;
			{
				std::lock_guard<std::mutex> lock(*serverQueueMutex);
				serverQueue.push(req_str);
			}
		}}catch(const std::exception& e ){
			std::cout<<"READ EXCEPTION: "<<e.what()<<std::endl;
		}
}
// SERVER - Processes client entity information. Ensures that the client is still connected and in a visible state
void Networking::processEntityInfo(std::string entityInfo,std::string& finalEntityMsg, std::vector<bool>& clientChanged){
	std::vector<std::string> vectorOfEntityStrings;
	std::string msgOrigin = eraseSpace(entityInfo.substr(entityInfo.find('(')+1, entityInfo.find(')') - entityInfo.find('(') -1));
	entityInfo.erase(0, entityInfo.find(')')+1);
	
	if(msgOrigin != "" && server_clientVsPorts.count(msgOrigin) != 1){
		nuOfClients+=1;
		server_clientVsPorts[msgOrigin] = std::vector<std::string>{std::to_string((5555+ 2 * std::stoi(msgOrigin))), std::to_string((5556 + 2 * std::stoi(msgOrigin)))};
		clientLastAlive[msgOrigin] = int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
	} else if (server_clientVsPorts.count(msgOrigin) == 1) {
		clientLastAlive[msgOrigin] = int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
	}
}

// SERVER - Spinserver publishs information to clients using the serverQueue (other client infromation) and entityList (moving platforms)
void Networking::serverPublish(std::mutex* serverQueueMutex, std::mutex* entityMutex){
	try{
		std::string finalEntityMsg = "";
		std::vector<bool> clientChanged(nuOfClients, false);
		{
			// Puts client information on delivery message, clears queue
			std::unordered_map<std::string, bool> entityVsSeen;
			std::scoped_lock lock(*serverQueueMutex, *entityMutex);
			std::string finalEntityMsg;
			while(!serverQueue.empty()){
				std::string entityInfo = serverQueue.front();
				if(entityVsSeen.count(entityInfo) == 0){
					entityVsSeen[entityInfo] = true;
					processEntityInfo(entityInfo, finalEntityMsg, clientChanged);
					serverQueue.pop();
				}
				else {
					serverQueue.pop();
					continue;
				}
				// std::queue<std::string> empty;
				// std::swap(serverQueue, empty);
				// break;
			}

			// Only Loads clients that are still in the server, this handles the graceful disconnects
			for(auto const& x: server_clientVsPorts){
				int currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				if (currentTime - clientLastAlive[x.first] < 2000)
					finalEntityMsg += x.first+":["+x.second[0]+","+x.second[1]+"];";
			}

			for(int i=0;i<entityList.size();i++){
				float xcoord = entityList[i]->getLocation()[0];
				float ycoord = entityList[i]->getLocation()[1];
				if(entityList[i]->getType() == PLATFORM)
					finalEntityMsg+="PLATFORM"+std::to_string(i)+":["+std::to_string(xcoord)+","+std::to_string(ycoord)+"];";
				else	
					finalEntityMsg+="ENEMY"+std::to_string(i)+":["+std::to_string(xcoord)+","+std::to_string(ycoord)+"];";
			}
			zmq::message_t update(finalEntityMsg.size());
			memcpy(update.data(), finalEntityMsg.data(), finalEntityMsg.size());
			//std::cout<<"server sends "<<finalEntityMsg<<std::endl;
			pub_socket->send(update, zmq::send_flags::dontwait);
		}
		
	}catch(const std::exception& e){
		// std::cout<<"PUB EXCEPTION :"<<e.what()<<std::endl;
	}

}

// SERVER - Spin server used by the server process
void Networking::spinServer(Networking* server, std::string pubIp, std::string pullIp){
	try{
		zmq::context_t context(1);

		// PUB socket for broadcasting messages
		pub_socket = new zmq::socket_t(context, zmq::socket_type::pub);
		pub_socket->bind(pubIp);

		std::cout << "Server is listening on port 5556 and Publishing on 5555..."<<std::endl;

		// PULL socket for detecting subscribers
		pull_socket = new zmq::socket_t(context, zmq::socket_type::pull);
		pull_socket->bind(pullIp);

		// Set timeline for moving platforms
		Timeline* platformTimeLine = new Timeline(NULL, 5);
		float currentTime = platformTimeLine->getTime();
		float lastTime = currentTime;

		std::mutex *serverQueueMutex = new std::mutex();
		std::mutex *entityMutex = new std::mutex();
		while(true){

			// Read from and write to clients in a separate thread
			std::thread serverSpawnReadThread(serverReadWrapper, server, serverQueueMutex);
			std::thread serverSpawnPubThread(serverPubWrapper, server, serverQueueMutex, entityMutex);
		
			// Main thread moves the platforms before joining everything and sleeping
			currentTime = platformTimeLine->getTime();
			float deltaTime = currentTime - lastTime;
			{

				std::lock_guard<std::mutex> lock(*entityMutex);
				movingPlatform(entityList, 0.05);
			}
			lastTime = currentTime;
			
			// Join threads from the pull and publisher 
			serverSpawnReadThread.join();
			serverSpawnPubThread.join();

			std::this_thread::sleep_for(std::chrono::milliseconds(60));
		}

	}
	catch(const std::exception& e){
		// std::cout<<"HERE :"<<e.what();
	}

}

// CLIENT - Client connects and binds to the subscriber socket and push socket
// Several buffers are used to send/recieve messages to server
void Networking::connect(Networking* client, std::string subIp, std::string pushIp, zmq::context_t* context, std::string connectMsg, std::string *sendBuffer, std::string *recvBuffer, std::mutex* sendBufferMutex, std::mutex* recvBufferMutex){
	try{	
		// Bind to new sockets
		sub_socket = new zmq::socket_t(*context, zmq::socket_type::sub);
		sub_socket->connect(subIp);
		sub_socket->setsockopt(ZMQ_SUBSCRIBE, "", 0); 

		push_socket = new zmq::socket_t(*context, zmq::socket_type::push);
		push_socket->connect(pushIp);

		std::string registration_msg = connectMsg;
		zmq::message_t req_msg(registration_msg.size());
		memcpy(req_msg.data(), registration_msg.data(), registration_msg.size());

		push_socket->send(req_msg, zmq::send_flags::none);

	}
	catch(const std::exception& e){
		// std::cout<<"connection exception : "<<e.what()<<std::endl;
	}
}

// CLIENT
void Networking::recvClientFunction(std::mutex* recvBufferMutex, std::string* recvBuffer){
	try{
		zmq::message_t message;
		while(sub_socket->recv(message, zmq::recv_flags::dontwait)){
			{
				std::lock_guard<std::mutex> lock(*recvBufferMutex);
				std::string received_msg(static_cast<char*>(message.data()), message.size());
				*recvBuffer = received_msg;
			}
		}
	}
	catch(const std::exception& e){
		std::cout<<"recv client function exception :"<<e.what()<<std::endl;
	}
}

// CLIENT
void Networking::sendClientFunction(std::mutex* sendBufferMutex, std::string* sendBuffer){
	try{
		{
			std::lock_guard<std::mutex> lock(*sendBufferMutex);
			zmq::message_t req_msg(sendBuffer->size());
			memcpy(req_msg.data(), sendBuffer->c_str(), sendBuffer->size());
			push_socket->send(req_msg, zmq::send_flags::none);
		}
	}
	catch(const std::exception& e){
		std::cout<<"send client function exception :"<<e.what()<<std::endl;
	}
}

// CLIENT
void Networking::pubClientFunction(std::mutex* pubMutex, std::string* pubMsg, zmq::socket_t* pub_socket){
	{	
		std::lock_guard<std::mutex> lock(*pubMutex);
		zmq::message_t update(pubMsg->size());
		memcpy(update.data(), pubMsg->data(), pubMsg->size());
		pub_socket->send(update, zmq::send_flags::dontwait);	
	}	
}

// CLIENT
void Networking::subClientFunction(std::mutex *subBufferMutex, std::string *subBuffer, std::string port){
	try{
		zmq::message_t message;
		zmq::socket_t* sub_socket = clientVsSocket[port];
		char endpoint[256];
    	size_t endpoint_size = sizeof(endpoint);
		sub_socket->getsockopt(ZMQ_LAST_ENDPOINT, &endpoint, &endpoint_size);
		while(sub_socket->recv(message, zmq::recv_flags::dontwait)){
			{
				std::lock_guard<std::mutex> lock(*subBufferMutex);
				std::string received_msg(static_cast<char*>(message.data()), message.size());
				*subBuffer = received_msg;
			}
		}
	}
	catch(const std::exception& e){
		std::cout<<"recv client function exception :"<<e.what()<<std::endl;
	}
}

// CLIENT
void Networking::sendToServer(std::string connectMsg){
	try{	
		std::string registration_msg = connectMsg;
		zmq::message_t req_msg(registration_msg.size());
		memcpy(req_msg.data(), registration_msg.data(), registration_msg.size());

		push_socket->send(req_msg, zmq::send_flags::none);

	}
	catch(const std::exception& e){
		// std::cout<<"connection exception : "<<e.what()<<std::endl;
	}
}
