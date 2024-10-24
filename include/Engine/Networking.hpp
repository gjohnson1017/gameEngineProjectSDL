#pragma once
#include "Physics.hpp"
#include "Entity.hpp"
#include "Thread.hpp"
#include <mutex>
#include <zmq.hpp>
#include <unordered_map>

class Networking{
	public:

		Networking();

		// Constructs Network server
		Networking(std::string uniqueId, bool isServer);
		// Constructs Network server with the ID and the ports that it will communicate with. 
		Networking(std::string uniqueId, std::string port1Val, std::string port2Val);

		// Spin server takes the port numbers and handles client communication
		void spinServer(Networking*,std::string, std::string);
		// Clients connect
		void connect(Networking*,std::string, std::string, zmq::context_t*, std::string, std::string*, std::string*, std::mutex*, std::mutex*);
		bool disconnect(); // Used to handle graceful disconnects
		void sendToServer( std::string ); // Client info -> Server
		void clientFunction(zmq::socket_t sub_socket); 
		void serverRead(std::mutex*); // Server <- Client info
		void processEntityInfo(std::string, std::string&, std::vector<bool>&); // Server processes client info on entities
		void serverPublish(std::mutex*, std::mutex*); // Server info -> Client
		void recvClientFunction(std::mutex*,std::string*); // Client info <- Server
		void sendClientFunction(std::mutex*,std::string*); // Client info -> Server
		void pubClientFunction(std::mutex*,std::string*,zmq::socket_t*); // Client publishes to other clients 
		void subClientFunction(std::mutex *subBufferMutex, std::string *subBuffer, std::string port); // Client subscribes to server

		// Used by client to read server information. Tells client where to render moving objects and buddies
		void parseServerMessage(std::vector<std::string>& vectorOfPorts);

		// Destructs sockets
		~Networking() {
			if(isServer)
			{
				pull_socket->close();
				pub_socket->close();
				delete pull_socket; 
        		delete pub_socket;

			}
			else{
				push_socket->close();
				sub_socket->close();
				delete push_socket;
        		delete sub_socket;

			}
				
			
		}

		// Map of all sockets (value) based on their port String (key)
		std::unordered_map<std::string, zmq::socket_t *> clientVsSocket;

		void createSocket(zmq::context_t* context , std::string port){
			clientVsSocket[port] = new zmq::socket_t(*context, zmq::socket_type::sub);
			//std::cout<<"ESTABLISHING NEW CONNECTION >>>"<<"tcp://localhost:"+port<<std::endl;
			clientVsSocket[port]->connect("tcp://localhost:"+port);
			clientVsSocket[port]->setsockopt(ZMQ_SUBSCRIBE, "", 0); 
		}

	private:
		// Each client gets their own buffer to avoid blocking and mutex by using the same section of memory
		std::vector<std::string*> serverBuffer;

		bool isServer;
		std::string uniqueId;
		int nuOfClients;

		std::vector<Entity*> entityList;

		std::unordered_map<std::string, std::vector<std::string>> server_clientVsPorts;
		std::unordered_map<std::string, int> clientLastAlive;

		zmq::socket_t *pull_socket;
		zmq::socket_t *sub_socket;
		zmq::socket_t *pub_socket; // this attribute is used by client too...they dont share memory though because we use different MEMORY SLICES WITH NEW INSTANCES OF NETWORKING
		zmq::socket_t *push_socket;
		
		std::vector<zmq::socket_t  *> clientSockets;

};
