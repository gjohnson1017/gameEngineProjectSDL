#pragma once
#include <mutex>


class Timeline {
	public:
		Timeline();
		Timeline(Timeline* p_anchor, int64_t p_tic);
		float getTime();
		void pause();
		void unpause();
		void changeTic(uint64_t p_tic){ tic = p_tic;}
		uint64_t getTics(){return tic;}
		bool isNormal(uint64_t &deltaTime);
		void togglePause(){ if(paused){unpause();}else{pause();}}
		float deltaTime();
		Timeline* getAnchor(){return anchor;}
		void setParentTime();
		float getStartTime(){return start_time;}


	private: 
		std::mutex m;
		uint64_t start_time;
		uint64_t paused_time;
		uint64_t pause_elapsed_time;
		uint64_t tic;
		bool paused;
		Timeline *anchor;

		float fixedTime;

};
