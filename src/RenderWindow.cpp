#include "Engine/RenderWindow.hpp"
#include "Engine/Entity.hpp"
#include "Engine/Color.hpp"

#include <iostream>
#include <thread>

// The render window constructor takes in title, width, and height as parameters 
// the window is then initialized with all the paramters

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
:_title(p_title), _width(p_w), _height(p_h){
    _closed = !init();
}


// Initializes the window and renderer, sends an error message if there is an error
bool RenderWindow::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL.\n";
        return 0;
    }

    window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "Failed to create window.\n";
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer.\n";
    }
    return true;
}

const float ASPECT_RATIO = 16.0f / 9.0f;

// Checks if the window is exited
float RenderWindow::pollEvents(int default_width) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            _closed = true;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int old_width = _width;
                _width = event.window.data1;
                _height = _width / ASPECT_RATIO;
                SDL_SetWindowSize(window, _width, (int)(_width / ASPECT_RATIO));
            }
            break;

        default:
            break;
        }
    }
    if (RenderWindow::propScale) {
        return (float)_width;
    }
    else {
        return default_width;
    }
}

void RenderWindow::renderEntity(Entity* p_e, struct Color color, float scale, float adjustFactor){
    SDL_Rect srcrect;
    std::vector<int> entityCoords = p_e->getLocation();

    srcrect.x = entityCoords[0] * scale - adjustFactor;
    srcrect.y = entityCoords[1] * scale;
    srcrect.w = p_e->getDimensionW() * scale;
    srcrect.h = p_e->getDimensionH() * scale;
    //scale the destination rectangle based on screen resolution, currently unscaled

    // SDL_Rect dstrect;
    // dstrect.x = entityCoords[0] * scale;
    // dstrect.y = entityCoords[1] * scale;
    // dstrect.w = p_e->getDimensionW() * scale;
    // dstrect.h = p_e->getDimensionH() * scale;
    
    if(p_e->getTexture()!=NULL){
        //SDL_RenderCopy(renderer, p_e->getTexture(), &srcrect, &dstrect);
    }else{
        SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );
        SDL_RenderFillRect( renderer, &srcrect );
        //SDL_RenderClear(renderer);
    }
    
}

// renderClear first fixes the backGround color of the screen and then clears the renderer with the drawing color
void RenderWindow::renderClear(struct Color bgColor){
    SDL_SetRenderDrawColor( renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a );
    SDL_RenderClear(renderer);
}

void RenderWindow::renderScene(){
    SDL_RenderPresent(renderer);
}

void RenderWindow::toggleScaling() {
    RenderWindow::propScale= !RenderWindow::propScale;
}

RenderWindow::~RenderWindow(){

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cout<<"Sanity check destruction"<<std::endl;
}
