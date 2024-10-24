/*
credits for initial template creation of RenderWindow.hpp, RenderWindow.cpp, Entity.hpp, Entity.cpp and folder structure ideas:
https://youtube.com/playlist?list=PL2RPjWnJduNmXHRYwdtublIPdlqocBoLS&feature=shared
*/

#pragma once

#include <SDL2/SDL.h>
#include <thread>
//#include <SDL2/SDL_image.h>

#include "Entity.hpp"
#include "Color.hpp"


class RenderWindow {
    public:
        RenderWindow();
        RenderWindow(const char* p_title, int p_w, int p_h);  
        ~RenderWindow();
        SDL_Texture* loadTexture(const char* p_filePath);
        
        static void toggleScaling();
        void renderScene();//paint the whole screen with whatever needs to be rendered
        void renderEntity(Entity* p_e, struct Color p_c, float scale, float);//adds entity to the backbuffer
        void renderClear(struct Color p_color);
        int getWidth(){ return _width;}

        inline bool isClosed() const {return _closed; }
        float pollEvents(int default_width);

        static float getDeltaTime(float &previousTime);
    private:
        bool init();
        bool _closed = false;
        static bool propScale;

        std::string _title;
        int _width;
        int _height;

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        int scalingMode = 0;


};