#include <iostream>
#include <math.h>
#include <SDL2/SDL.h>

#include "Engine/Physics.hpp"

Physics::Physics(Coordinates* p_coord, Dimensions* p_dims, float p_mass, std::vector<float> p_acc, std::vector<bool> p_collision, std::vector<float> p_drag){
    coordinates = p_coord;
    //originalCoord = p_coord;
    dimensions = p_dims;
    mass = p_mass;
    acceleration = p_acc;
    force = {p_mass * p_acc[0], p_mass * p_acc[1]};
    velocity = {0.0f, 0.0f};
    drag = p_drag;

    collision = p_collision;
    
    //WORLD_DIMENSIONS = 2;
}

// all the physics change to an entity in a particular frame takes place in changeState. 
// physics applications are all modeled currently as forceApplied to an object to initiate a change in state
// after force is applied in a frame, the complete motion is then modeled by acceleration induced in the entity (which keeps the entity in motion unless decelerated )
// timeStep helps apply physics while accounting for variable framerate

void Physics::changeState(std::vector<float> forceApplied, float constantTime, int itr){

    //collapse all the math later, not all of these variable creations are necessary
    //acceleration[1] = gravity;
   
    for(int i=0;i<WORLD_DIMENSIONS;i++){
        //drag can be modelled later
        if(itr == 0) force[i] = mass * acceleration[i] - ( drag[i] * velocity[i] * velocity[i] * 1.2 * 0.5) + forceApplied[i];
        else force[i] = mass * acceleration[i]  - ( drag[i] * velocity[i] * velocity[i] * 1.2 * 0.5);  

        if(i == 1) force[i] += mass * gravity; 

        //force[i] = mass * acceleration[i] - ( drag[i] * velocity[i] * velocity[i] * 1.2 * 0.5) + forceApplied[i];

        //std::cout<<forceApplied[i]<<" "<<velocity[i]<<std::endl;  
            
        //velocity verlt integration used here to model the physics, credits: http://buildnewgames.com/gamephysics/
        float lastAcc = acceleration[i];
        float newAcc =  (force[i] )/mass;
        float avgAcc = (newAcc + lastAcc) / 2;
        velocity[i] += avgAcc * constantTime; 
        float coordinateUpdate = (velocity[i] * constantTime + (0.5 * lastAcc * constantTime * constantTime));
        int coordinateUpdateVal = coordinateUpdate >= 0 ? ceil(coordinateUpdate) : floor(coordinateUpdate);
        coordinates->coordinates[i] += coordinateUpdateVal;
        acceleration[i] = newAcc;

        // helps keep the entity within bounds of world all the time when a change in state is induced
        // if(i == 0) {
        //     coordinates->coordinates[i] = std::max(Physics::WORLD_BOUND_X[0], coordinates->coordinates[i]);
        //     coordinates->coordinates[i] = std::min((float)Physics::WORLD_BOUND_X[1], (coordinates->coordinates[i] + dimensions->dimensions[i])) - dimensions->dimensions[i];
        //     velocity[i] = 0; // we dont have any friction so we halt the player when no x forces are given
        //     acceleration[i] = 0; // we dont have any friction so we halt the player when no x forces are given
        // }
        // else if(i==1){
        //     coordinates->coordinates[i] = std::max(Physics::WORLD_BOUND_Y[0], coordinates->coordinates[i]);
        //     coordinates->coordinates[i] = std::min((float)Physics::WORLD_BOUND_Y[1],(coordinates->coordinates[i] + dimensions->dimensions[i])) - dimensions->dimensions[i];
        // }    
    }
       
    
    
}

// instead of inducing patterns as acceleration which would make the motion of entities janky, entities with patterns use a seperate executePattern function
// once the bounds of the pattern are hit, the velocity vector's direction would switch to continue with the pattern execution

void Physics::executePattern(float deltaTime){
    //std::cout<<"inside execute pattern"<<std::endl;
    for(int i =0; i<WORLD_DIMENSIONS;i++){
        coordinates->coordinates[i] += velocity[i] > 0 ? ceil(velocity[i] * deltaTime) : floor(velocity[i] * deltaTime);
        if(pattern.size()!=0 && (coordinates->coordinates[i] <= pattern[i][0] || coordinates->coordinates[i] >= pattern[i][1])) velocity[i]*=-1;         
    }
}
