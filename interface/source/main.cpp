#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>
#include <string>

struct SDLState{
  SDL_Window *window;
  SDL_Renderer *renderer;
};

void CleanUp(SDLState &state);

int main(int argc, char *argv[]) {
    SDLState state;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error initializing SDL3", nullptr);
        return 1;
    }

    int width = 800;
    int height = 600;

    state.window = SDL_CreateWindow("azulindo", width, height, 0);
    if (!state.window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating window", nullptr);
        CleanUp(state);
        return 1;
    }

    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating renderer", nullptr);
        CleanUp(state);
        return 1;
    }

    bool running = true;
    while (running) {
      SDL_Event event{0};
      while (SDL_PollEvent(&event)) {
          switch (event.type) {
              case SDL_EVENT_QUIT: {
                  running = false;
                  break;
              }
    
          }
      }
      SDL_SetRenderDrawColor(state.renderer, 0, 0, 125, 255);
      SDL_RenderClear(state.renderer);
      SDL_RenderPresent(state.renderer);
    }
    CleanUp(state);
    return 0;
}

void CleanUp(SDLState &state) { 
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}