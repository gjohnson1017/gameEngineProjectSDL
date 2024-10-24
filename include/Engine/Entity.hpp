#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "Engine/Physics.hpp"
#include "Engine/KeyBindings.hpp"
#include "Engine/Timeline.hpp"

// Enum written to differently model specific entity types and their behaviour
// Each entity created with an entity type would help devs group each entity type behaviour and model its functionality

enum EntityType{
    PLAYER,
    BUDDIES,
    PLATFORM,
    PLATFORM_STATIC_1,
    PLATFORM_STATIC_2,
    PLATFORM_STATIC_3,
    GROUND,
    ENEMY,
    NPC,
    ZONE
};


class Entity{
    public:
        Entity();
        // Entity(int p_x, int p_y, float p_w, float p_h);
        // Entity(int p_x, int p_y, float p_w, float p_h, SDL_Texture* p_texture);
        //Entity(std::vector<int> p_coords, std::vector<float> p_dimension, SDL_Texture* p_texture);
        Entity(EntityType p_type,std::vector<int> p_coords, std::vector<float> p_dimensions);
        Entity(EntityType p_type,std::vector<int> p_coords, std::vector<float> p_dimensions, std::vector<float> p_velocity, std::vector<float> patternLimits_x, std::vector<float> patternLimits_y);
        Entity(EntityType, std::vector<int> p_coords, std::vector<float> p_dimension, float p_mass, KeyBindings* keybind = NULL, std::vector<float> p_acc= {0.0f, 0.0f}, std::vector<bool> collision = {false, false}, std::vector<float> drag = {0.0f, 0.0f});
        ~Entity() { std::cout<<"Sanity Check of entity destruction"<<std::endl; }
        float getDimensionW();
        float getDimensionH();
        Physics* getEntityPhysics(){return entityPhysics;}
        KeyBindings* getKeyBindings() {return keybinds;}

        std::vector<int> getLocation() { return coordinates->coordinates; }
        void setLocation(Coordinates* p_coord) { coordinates = p_coord; entityPhysics->setLocation(p_coord); }
        void setDimensions(Dimensions* p_dims) { dimensions = p_dims; entityPhysics->setDimension(p_dims); }
        KeyBindings* getKeyBinds() { return keybinds; }
        std::vector<float> actionToPhy(SDL_Scancode sdlcode, float deltaTime, float force, std::vector<float>);
        void deleteEntity() { delete this; }

        static std::map<Entity*, std::string> detectCollision(Entity*, std::vector<Entity*>,float deltaTime);
        static void translateEntities(Entity* p_e, std::vector<Entity*> entityList, float deltaTime);

        void setEntityTimeLine(Timeline* p_entTimeLine){ entityTimeLine = p_entTimeLine;}
        Timeline* getEntityTimeLine(){return entityTimeLine;}

        SDL_Texture* getTexture();
        SDL_Rect getCurrentFrame();
        
        EntityType getType() {return type;}

    private:
        //std::vector<int> coordinates;//pass this as reference to physics
        Coordinates* coordinates;
        Dimensions* dimensions;
        //std::vector<float> dimensions;
        SDL_Rect currentFrame;
        SDL_Texture* texture;
        Physics* entityPhysics;
        KeyBindings* keybinds;
        EntityType type;
        Timeline* entityTimeLine;
        
};

