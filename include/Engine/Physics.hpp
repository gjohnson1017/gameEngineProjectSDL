#pragma once
#include <iostream>
#include <vector>


/*credit: article refered to read some considerations http://buildnewgames.com/gamephysics/
          got an idea of timesteps and delta time from this video https://www.youtube.com/watch?v=pctGOMDW-HQ 
          effects of shape on drag https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/shape-effects-on-drag/
*/

class Coordinates{
    public:
        Coordinates();
        Coordinates(std::vector<int> p_coords): coordinates(p_coords) {}
        
        //Coordinates(std::vector<int>& p_coords): coordinates(p_coords) {}
        
        std::vector<int> getLocation(){return coordinates;}
        std::vector<int> coordinates;

};

class Dimensions{
    public:
        Dimensions();
        Dimensions(std::vector<float>& p_dims): dimensions(p_dims) {}
        //Coordinates(std::vector<int>& p_coords): coordinates(p_coords) {}
        
        std::vector<float> getDimensions(){return dimensions;}
        std::vector<float> dimensions;

};

class Physics{
    public:
        Physics();
        Physics(Coordinates* coords,Dimensions* dims, std::vector<float> p_velocity):velocity(p_velocity), coordinates(coords), dimensions(dims){}
        Physics(Coordinates* coords,Dimensions* dims, float p_mass, std::vector<float> p_acc, std::vector<bool> collision, std::vector<float> drag);

        float getVelocityComponent(int index){ if(index < WORLD_DIMENSIONS) return velocity[index]; return 0.0f;}
        float getAccelerationComponent(int index){ if(index < WORLD_DIMENSIONS) return acceleration[index]; return 0.0f;}
        float getForceComponent(int index){ if(index < WORLD_DIMENSIONS) return force[index]; return 0.0f;}
        float getMass() {return mass;}

        
        
        void changeState(std::vector<float> forceApplied, float constantTimeStep, int itr); // does velocity verlt integration -- something i learnt from the first credits link
        void executePattern(float timeStep);

        void setForce(std::vector<float> p_force) { force = p_force; }  
        void setAcceleration(std::vector<float> p_accl) { acceleration = p_accl; }
        void setVelocity(std::vector<float> p_vel) { velocity = p_vel; }
        void setLocation(Coordinates* p_coord) {coordinates = p_coord; }
        void setDimension(Dimensions* p_dims) {dimensions = p_dims;}
        

        void addForce(int index, float p_f) { force[index] += p_f; }
        
        
        //static void setGravity(float p_g);
        //static float getGravity() { return Physics::gravity; }

        //static void setWorldDim(int dims);
        
        static float gravity;
        static int WORLD_DIMENSIONS;
        static std::vector<int> WORLD_BOUND_X;
        static std::vector<int> WORLD_BOUND_Y; 
        void setPattern(std::vector<float> p_x, std::vector<float> p_y) {  pattern.push_back(p_x); pattern.push_back(p_y); }
        void overridePattern(std::vector<std::vector<float>> newPattern) {  pattern = newPattern; }
        bool hasPattern() { return (pattern[0].size() > 1); }
        std::vector<std::vector<float>> getPattern() { return pattern; }

    private:
        Coordinates* coordinates;

        Dimensions* dimensions;
        
        std::vector<bool> collision;

        float mass;
        std::vector<float> velocity;  
        std::vector<float> acceleration;
        std::vector<float> force;
        
        /* some thoughts on drag -- if we model water, then we can define that as an entity in space at a coordinate that has some drag force which will affect player's movement
            similarly, friction can be modeled as a drag i guess. friction modeled as a drag however is a reductive way of looking at friction
        */
        std::vector<float> drag; // this is more like the forces it exerts on all other objects it collides with       
        
        std::vector<std::vector<float>> pattern;


        
           
        
        
};

