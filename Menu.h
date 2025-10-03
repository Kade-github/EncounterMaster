#ifndef _MENU_H
#define _MENU_H

#include <SDL3/SDL.h>
#include <vector>
#include "Encounter.h"
#include "Download.h"
#include "CreatureList.h"
#include "CreatureEdit.h"
#include "EncounterEdit.h"

struct CreaturePage {
  std::vector<dnd::Creature> creatures;
  int page_number;
};

struct ToolState {
  SDL_Window* window;
  SDL_Renderer* renderer;

  bool should_download_creatures = false;
  bool is_downloading_creatures = false;
  std::vector<dnd::Creature> creatures;

  bool reload_creatures = false;

  // Creature list
  std::vector<dnd::Creature> filtered_creatures;
  char creature_filter[256] = "";
  int creature_page = 0;
  int creature_selected_index = -1;
  CreaturePage page;

  // Creature edit

  dnd::Creature current_creature;
  bool clear_image = false;
  bool is_downloading_image = false;
  bool image_error = false;
  SDL_Texture* current_creature_texture = nullptr;

  bool show_error_popup = false;
  std::string error_message;

  dnd::Encounter current_encounter;

  bool encounter_planner = false;
  bool encounter_battler = false;
};

void menuDraw(ToolState* state);
#endif