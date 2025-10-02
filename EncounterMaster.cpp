#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

struct ToolState {
  SDL_Window* window;
  SDL_Renderer* renderer;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  ToolState* state = new ToolState;
  if (!state) {
    SDL_Log("Failed to allocate ToolState");
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    delete state;
    return SDL_APP_FAILURE;
  }

  state->window =
      SDL_CreateWindow("Encounter Master", 1280, 720, SDL_WINDOW_RESIZABLE);

  if (!state->window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    delete state;
    return SDL_APP_FAILURE;
  }

  state->renderer = 
      SDL_CreateRenderer(state->window, NULL);

  if (!state->renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_DestroyWindow(state->window);
    SDL_Quit();
    delete state;
    return SDL_APP_FAILURE;
  }

  *appstate = state;

  SDL_SetRenderVSync(state->renderer, -1);  // Enable VSync if available

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // Exit the application
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) { 
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  ToolState* state = (ToolState*)appstate;
  if (state) {
    if (state->renderer) {
      SDL_DestroyRenderer(state->renderer);
    }
    if (state->window) {
      SDL_DestroyWindow(state->window);
    }
    delete state;
  }
}