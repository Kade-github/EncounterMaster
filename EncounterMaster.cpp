#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <curl/curl.h>

#include <filesystem>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"

#include "Menu.h"

bool doCreaturesExist()
{
  // Check `creatures/` directory for any .json files
  const std::filesystem::path creatures_dir = "creatures";
  if (!std::filesystem::exists(creatures_dir) || !std::filesystem::is_directory(creatures_dir)) {
    return false;
  }
  for (const auto& entry : std::filesystem::directory_iterator(creatures_dir)) {
    if (entry.path().extension() == ".json") {
      return true;  // Found at least one .json file
    }
  }
  return false;  // No .json files found
}

void logOutput(void* userdata, int category, SDL_LogPriority priority,
                        const char* message) {
  printf("SDL Log [%d] (%d): %s\n", category, priority, message);

  if (category == SDL_LOG_CATEGORY_ERROR)
  {
    ToolState* state = (ToolState*)userdata;
    if (!state) return;
    state->show_error_popup = true;
    state->error_message = message;
  }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  ToolState* state = new ToolState;
  state->encounter_planner = true;
  state->encounter_battler = false;
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
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForSDLRenderer(state->window, state->renderer);
  ImGui_ImplSDLRenderer3_Init(state->renderer);

  SDL_SetLogOutputFunction(logOutput, state);

  // Check if creatures exist; if not, set flag to download them
  state->should_download_creatures = !doCreaturesExist();

  if (!state->should_download_creatures)
  {
    state->creatures = parse_creatures_from_directory("creatures");
    if (state->creatures.empty())
    {
      state->should_download_creatures = true;
    }
  } else {
    SDL_Log("Creature data not found, will prompt to download.");
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

  ImGui_ImplSDL3_ProcessEvent(event);

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // Exit the application
  }
  return SDL_APP_CONTINUE;
}

void downloadCreaturesMenu(ToolState* state)
{ 
  if (!state->is_downloading_creatures)
  {
    ImGui::Begin("Creatures Missing", nullptr,
                 ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Creature data is missing. Would you like to download it?");
    if (ImGui::Button("Download Creatures"))
    {
      state->is_downloading_creatures = true;
      downloadCreatures([state]() {
        state->is_downloading_creatures = false;
        state->should_download_creatures = false;
        state->creatures = parse_creatures_from_directory("creatures");

        if (state->creatures.empty())
        {
          state->should_download_creatures = true;
        }
      });
    }
    ImGui::End();
    return;
  }

  curlStatus status = downloadCallbacks();
  ImGui::Begin("Downloading Creatures", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking |
                   ImGuiWindowFlags_NoCollapse);
  ImGui::Text("Downloading creature data...");
  ImGui::ProgressBar((float)status.downloaded / (float)status.total,
                     ImVec2(300, 0));
  ImGui::End();
}

SDL_AppResult SDL_AppIterate(void* appstate) { 
  ToolState* state = (ToolState*)appstate;

  SDL_Renderer* renderer = state->renderer;
  ImGuiIO& io = ImGui::GetIO();
  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // Entry point for rendering the GUI

  menuDraw(state);

  if (state->should_download_creatures) 
      downloadCreaturesMenu(state);

  if (state->show_error_popup) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::OpenPopup("error");
    state->show_error_popup = false;
  }

  if (ImGui::BeginPopup("error"))
  {
    ImGui::Text("Error: %s", state->error_message.c_str());
    if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
  }
  
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