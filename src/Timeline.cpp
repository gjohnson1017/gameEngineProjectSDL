#include <chrono>
#include <iostream>
#include "Engine/Timeline.hpp"


Timeline::Timeline(Timeline* p_anchor, int64_t p_ticks): anchor(p_anchor), tic(p_ticks) {
	this->paused = false;
	this->start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	fixedTime = start_time;
	if(p_anchor != NULL){
		this->start_time = p_anchor->getTime();	
	}
	
}

void Timeline::setParentTime(){
	fixedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

float Timeline::getTime(){
	if(paused){
		return paused_time/(float)tic;
	}
	if(anchor == NULL){
		//std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()<<std::endl;
		//std::cout<<(float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()<<std::endl;
		return fixedTime;
	}
	else{
		float localCurrentTime = (this->anchor->getTime() - start_time)/(float)tic;
		return localCurrentTime;
	}
}


void Timeline::pause(){
	paused = true;
	paused_time =  this->anchor != NULL ? this->anchor->getTime() : std::time(NULL);
	if(this->anchor != NULL) this->anchor->pause();
}

void Timeline::unpause(){
	std::cout<<"Unpaused "<<std::endl;
	paused = false;
	if(this->anchor!= NULL) this->anchor->unpause();
}

