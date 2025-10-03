#include "Menu.h"
#include <fstream>

#include <SDL3/SDL.h>

dnd::Encounter parse_encounter_from_file(void* s, const std::string& filename) {
  ToolState* state = (ToolState*)s;

  auto start = SDL_GetTicks();

  std::ifstream(f);
  f.open(filename);
  if (!f.is_open()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not open file: %s",
                 filename.c_str());
    return dnd::Encounter();
  }
  json j;

  try {
    j = json::parse(f);
  } catch (const json::parse_error& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "JSON parse error in file %s: %s",
                 filename.c_str(), e.what());
    return dnd::Encounter();
  }

  dnd::Encounter encounter;

  try {
    if (j.contains("name"))
      encounter.set_name(j.value("name", "Unnamed Encounter"));
    if (j.contains("creatures") && j["creatures"].is_array()) {
      std::vector<std::string> creatures =
          j["creatures"].get<std::vector<std::string>>();
      encounter.set_creatures(creatures);
    }
    if (j.contains("notes") && j["notes"].is_array()) {
      std::vector<std::string> notes = j["notes"].get<std::vector<std::string>>();
      encounter.set_notes(notes);
    }
  } catch (const std::exception& e) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error parsing encounter: %s",
                 e.what());
    return dnd::Encounter();
  }

  int i = 0;
  std::vector<std::string> notes =
      encounter.get_notes().value_or(std::vector<std::string>());
  for (const auto& creature_name : encounter.get_creatures()) {
    bool found = false;
    for (auto creature : state->creatures) {
      std::string name = creature.get_name().value_or("Unnamed Creature");
      name += " (" + creature.original_list + ") (" + std::to_string(i) + ")"; 
      if (name == creature_name) {
        if (i < notes.size())
            creature.set_notes(notes[i]);
        creature.id = i;  // Assign an ID based on order
        encounter.creature_objects.push_back(creature);
        found = true;
        break;
      }
    }
    if (!found) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "Creature '%s' not found in loaded creatures.",
                  creature_name.c_str());
    }
    i++;
  }

  auto end = SDL_GetTicks();

  SDL_Log("Time taken to parse encounter: %u ms", end - start);

  return encounter;
}

void save_encounter_to_file(const dnd::Encounter& encounter,
                            const std::string& filename) {
  json j = json::array();
  j = encounter.to_json();
  std::ofstream o(filename);
  o << j;
  o.close();

  SDL_Log("Encounter saved to %s", filename.c_str());
}
