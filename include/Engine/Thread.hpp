#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

class Thread{
    private:
        bool busy;
        int id;
	std::vector<std::mutex*> _mutex;
   
    public:
    
        Thread() {};
        Thread(int p_id):id(p_id){};
	Thread(std::vector<std::mutex*> p_mutex):_mutex(p_mutex){}

        void setMutex(std::mutex* p_mutex){ _mutex.push_back(p_mutex); }
        void setID(int p_id){ id = p_id; }
};
