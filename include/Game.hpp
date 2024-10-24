#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <math.h>
#include <random>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <zmq.h>
#include <unordered_set>

#include "Engine/RenderWindow.hpp"
#include "Engine/Physics.hpp"
#include "Engine/Entity.hpp"
#include "Engine/Timeline.hpp"
#include "Engine/Networking.hpp"


class Game{
    public: 
    // template public properties applicable for all games
    Game();
    ~Game();
    void togglePause(Timeline *p_time){p_time->togglePause();}
    Entity* getPlayerEntity(){ return player;}
    //void setWindow(RenderWindow &p_window){window = &p_window;}

    void prepareScene();
    std::vector<float> getInput(float);
    void applyPhysics(std::vector<float>, float);
    void sendHelloToServer();
    void initializeTimeline();
    float gameDelta();
    void spawnP2PClientThreads();
    void prepareNetworkVariables(std::string,std::string,std::string);
    void processServerInformation();
    void processPeerInformation();
    void renderScene(RenderWindow &window);
    void collisionDetection(float);
    void publishMessage();
    void mergeP2PThreads();
    void closeContext() { std::cout<<"closing context"; context.close(); }
    void setScale(RenderWindow &p_window);
    void closeAllThreads();
    void buildEntityIdVsIndexMap();
    void waitForCompletion();
    void aggregateDeltas(float);

    void setSpawnZones();
    void setDeadZones();

    void reSpawn();

    //template public attributes that are constants for the game -- can be constumized in the constructor
    // std::vector<int> WORLD_BOUND_X;
    // std::vector<int> WORLD_BOUND_Y; 
    int WORLD_DIMENSIONS;
    float gravity;

    // game specific functions

    private:
    // template private attributes
    std::vector<Entity*> entityList;
    Entity* player;
    std::unordered_map<std::string, Entity *> entityIdVsProp;
    std::unordered_map<std::string, int> entityIdVsIndex;
    float scale;
    Timeline *globalTime;
	Timeline *gameTime;
    float currentGameTime;
    float lastGameTime;
    float deltaGameAccumulator;
    RenderWindow* window;
     std::unordered_set<std::string> activeClients;

    /*
        GAME SPECIFIC ATTRIBUTES GO HERE
    */
    bool plateOn, enemyHit;
    int togPressed;
    Entity* dynamicPressurePlateGlobal;
    std::thread clientRecvSpawnThread;
	std::thread clientPubSpawnThread;
    std::vector<std::thread> clientSubThreads;
    std::vector<std::vector<int>> spawnZones;
    std::vector<std::vector<int>> deadZones;
    int frameNumber;

    // client attributes
    std::unordered_map<std::string, std::vector<std::string>> client_clientVsPort;

    zmq::socket_t* pub_socket;

    std::mutex *sendBufferMutex;
    std::mutex *recvBufferMutex;
    std::mutex *pubBufferMutex;
    std::mutex *subBufferMutex;

    std::string *sendBuffer;
    std::string *recvBuffer;
    std::string *pubBuffer;
    std::string *subBuffer;

    std::string clientUniqueId;

    std::string serverPort1;
    std::string serverPort2;

    std::string clientPort1;
    std::string clientPort2;

    zmq::context_t context;
    Networking* client;


    float totalDeltas;

    
};
