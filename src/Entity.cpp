#include <iostream>
#include <string>
#include <math.h>

#include "Engine/Entity.hpp"

// populateCurrentFrame helps model an entity as a rectangle (useful for hitboxes) and provides it with dimensions and coordinates
void populateCurrentFrame(Coordinates* coordinates, Dimensions* dimensions, SDL_Rect& currentFrame)
{
    currentFrame.x = coordinates->getLocation()[0];
    currentFrame.y = coordinates->getLocation()[1];
    currentFrame.w = dimensions->getDimensions()[0];
    currentFrame.h = dimensions->getDimensions()[1];
}


// following commented since texture is not used in this iteration of the engine. Future iterations of the engine will have this uncommented to add textures to entities

// constructor used when Entity does not have physics 
Entity::Entity(EntityType p_type,std::vector<int> p_coords, std::vector<float> p_dimensions){
    type = p_type;
    coordinates = new Coordinates(p_coords);
    dimensions = new Dimensions(p_dimensions);
    texture = NULL;
    populateCurrentFrame(coordinates, dimensions, currentFrame);   
}

// constructor used when Entity has pseudo-physics -- where there is only a pattern to execute 
Entity::Entity(EntityType p_type,std::vector<int> p_coords, std::vector<float> p_dimensions, std::vector<float> p_velocity, std::vector<float> patternLimits_x, std::vector<float> patternLimits_y){
    type = p_type;
    coordinates = new Coordinates(p_coords);
    dimensions = new Dimensions(p_dimensions);
    texture = NULL;

    entityPhysics = new Physics(coordinates,dimensions,p_velocity);
    entityPhysics->setPattern({p_coords[0] - patternLimits_x[0], p_coords[0] + patternLimits_x[1]}, {p_coords[1] - patternLimits_y[0], p_coords[1] - patternLimits_y[1]});
    populateCurrentFrame(coordinates, dimensions, currentFrame);   
}

// constructor used when Entity also "Has-a" physics to it
Entity::Entity(EntityType p_type,std::vector<int> p_coords, std::vector<float> p_dimensions, float p_mass, KeyBindings* p_keyBinds , std::vector<float> p_acc , std::vector<bool> collision, std::vector<float> drag){
    type = p_type;
    coordinates = new Coordinates(p_coords);
    dimensions = new Dimensions(p_dimensions);
    texture = NULL;
    
    entityPhysics = new Physics(coordinates,dimensions, p_mass, p_acc, collision, drag);
    keybinds = p_keyBinds;

    populateCurrentFrame(coordinates, dimensions, currentFrame);   
}


// actionToPhy takes in the SDL codes taken from current keyboard state and maps it to a physics change using the keyBings set
// keyBinds map raw input to action enum values

std::vector<float> Entity::actionToPhy(SDL_Scancode sdlcode, float deltaTime, float force, std::vector<float> forceApplied ){

    //std::cout<< "action to phy: " << sdlcode << std::endl;

    Action actionType = keybinds->getKeyBind(sdlcode);

    switch(actionType){
        case MOVE_UP:
            forceApplied[1] -= force;
            break;
        case MOVE_DOWN:
            forceApplied[1] += force;
            break;
        case MOVE_LEFT:
            forceApplied[0] -= force;
            break;
        case MOVE_RIGHT:
            forceApplied[0] += force;
            break;
    }

     return forceApplied;
}


float Entity::getDimensionW(){
    return dimensions->getDimensions()[0];
}

float Entity::getDimensionH(){
    return dimensions->getDimensions()[1];
}

SDL_Texture* Entity::getTexture(){
    return texture;
}

SDL_Rect Entity::getCurrentFrame(){
    return currentFrame;
}

//collision detection

//current collision is O(n) for one object and O(n*n) if applied for mulitple objects
//i can probably use a location hash or some trick to restrict the number of comparisons made...

//we have to differentiate collision from mere coordinate overlapping

//credits: https://stackoverflow.com/questions/20722531/c-sdl-collision-between-sprites
bool hasCollision(SDL_Rect a, SDL_Rect b){
    return !(b.x > (a.x+a.w) || 
           (b.x+b.w) < a.x || 
           b.y > (a.y+a.h) ||
           (b.y+b.h) < a.y);
}

//credits: https://stackoverflow.com/questions/5062833/detecting-the-direction-of-a-collision
std::map<Entity*, std::string> Entity::detectCollision(Entity* p_e, std::vector<Entity*> entityList, float deltaTime)
{   
    std::map<Entity*, std::string> collisionMap;
    for(int i=0;i < entityList.size();i++){

        if(entityList[i]!=NULL && p_e != entityList[i] && p_e->getEntityPhysics()!=NULL && entityList[i]->getEntityPhysics()!=NULL){
            int A_X = p_e->getLocation()[0];
            int A_Y = p_e->getLocation()[1];
            int A_W = p_e->getDimensionW();
            int A_H = p_e->getDimensionH();
            
            int B_X = entityList[i]->getLocation()[0];
            int B_Y = entityList[i]->getLocation()[1];
            int B_W = entityList[i]->getDimensionW();
            int B_H = entityList[i]->getDimensionH();

            int bottom_collision  = B_Y + B_H - A_Y;
            int top_collision = A_Y + A_H - B_Y;

            int left_collision  = A_X + A_W - B_X;
            int right_collision = B_X + B_W - A_X;
            
            SDL_Rect A;
            std::vector<int> entityCoords = p_e->getLocation();
            
            A.x = entityCoords[0];
            A.y = entityCoords[1];
            A.w = p_e->getDimensionW();
            A.h = p_e->getDimensionH();

            SDL_Rect B;
            entityCoords = entityList[i]->getLocation();
            
            B.x = entityCoords[0];
            B.y = entityCoords[1];
            B.w = entityList[i]->getDimensionW();
            B.h = entityList[i]->getDimensionH();

            if(!hasCollision(A,B)) {
                continue;
            }
            
            // when an entity is in collision with another entity, there are more than one direction of collision that the engine might detect
            // to get one particular direction of collision, we can check the amount of intersection in a particular direction using the entity bounds
            // more info on intersection and direction would be available in Appendix A of writeup
            if(top_collision < bottom_collision && top_collision < left_collision && top_collision < right_collision){
                //top
                collisionMap[entityList[i]] = "TOP";
            }
            else if(bottom_collision < top_collision && bottom_collision < left_collision && bottom_collision < right_collision){
                //down
                collisionMap[entityList[i]] = "BOTTOM";
            }
            else if(left_collision < top_collision && left_collision < bottom_collision && left_collision < right_collision){
                //left
                collisionMap[entityList[i]] = "LEFT";
            }
            else if(right_collision < top_collision && right_collision < bottom_collision && right_collision < left_collision){
                //right
                collisionMap[entityList[i]] = "RIGHT";
            }
            
        }
    }

    return collisionMap;
}