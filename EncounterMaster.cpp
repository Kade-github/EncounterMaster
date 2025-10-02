#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"

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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForSDLRenderer(state->window, state->renderer);
  ImGui_ImplSDLRenderer3_Init(state->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

  ImGui_ImplSDL3_ProcessEvent(event);

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // Exit the application
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) { 
  SDL_Renderer* renderer = ((ToolState*)appstate)->renderer;
  ImGuiIO& io = ImGui::GetIO();
  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();
  ImGui::Render();
  SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x,
                     io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y,
                              clear_color.z, clear_color.w);
  SDL_RenderClear(renderer);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  SDL_RenderPresent(renderer);

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