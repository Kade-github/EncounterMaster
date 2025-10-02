#ifndef _MENU_H
#define _MENU_H

#include <SDL3/SDL.h>
#include <vector>
#include "Creature.h"

struct ToolState {
  SDL_Window* window;
  SDL_Renderer* renderer;

  bool should_download_creatures = false;
  bool is_downloading_creatures = false;
  std::vector<dnd::Creature> creatures;

  bool show_error_popup = false;
  std::string error_message;
};

void menuDraw(ToolState* state);

#endif