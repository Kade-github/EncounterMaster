#include "Creature.h"
#include <fstream>
#include <SDL3/SDL.h>

dnd::Creature parse_creature(const json& j) {
  dnd::Creature creature;
  // Main fields
  creature.set_name(j.value("name", ""));
  creature.set_meta(j.value("meta", ""));

  // Stats
  creature.set_armor_class(j.value("armor_class", ""));
  creature.set_hit_points(j.value("hit_points", ""));
  creature.set_speed(j.value("speed", ""));
  creature.set_str(j.value("str", ""));
  creature.set_str_mod(j.value("str_mod", ""));
  creature.set_dex(j.value("dex", ""));
  creature.set_dex_mod(j.value("dex_mod", ""));
  creature.set_con(j.value("con", ""));
  creature.set_con_mod(j.value("con_mod", ""));
  creature.set_creature_int(j.value("int", ""));
  creature.set_int_mod(j.value("int_mod", ""));
  creature.set_wis(j.value("wis", ""));
  creature.set_wis_mod(j.value("wis_mod", ""));
  creature.set_cha(j.value("cha", ""));
  creature.set_cha_mod(j.value("cha_mod", ""));

  // Other fields
  creature.set_saving_throws(j.value("saving_throws", ""));

  creature.set_skills(j.value("skills", ""));
  creature.set_senses(j.value("senses", ""));
  creature.set_languages(j.value("languages", ""));
  creature.set_challenge(j.value("challenge", ""));

  // Actions and traits
  creature.set_traits(j.value("traits", ""));
  creature.set_actions(j.value("actions", ""));
  creature.set_legendary_actions(j.value("legendary_actions", ""));

  // Image URL
  creature.set_img_url(j.value("img_url", ""));

  // Combat-related fields
  creature.set_damage_immunities(j.value("damage_immunities", ""));
  creature.set_condition_immunities(j.value("condition_immunities", ""));
  creature.set_damage_resistances(j.value("damage_resistances", ""));
  creature.set_damage_vulnerabilities(j.value("damage_vulnerabilities", ""));
  creature.set_reactions(j.value("reactions", ""));

  return creature;
}

std::vector<dnd::Creature> parse_creatures_from_file(
    const std::string& filename) {
  std::ifstream f(filename);

  using json = nlohmann::json;

  auto startTime = SDL_GetTicks();
  json j; 
  try {
    j = json::parse(f);
  }
  catch (const json::parse_error& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "JSON parse error in file %s: %s",
                 filename.c_str(), e.what());
    return {};
  }

  auto endTime = SDL_GetTicks();

  SDL_Log("Time taken to parse JSON: %u ms", endTime - startTime);
  std::vector<dnd::Creature> creatures;
  for (const auto& item : j) {
    creatures.push_back(parse_creature(item));
  }

  return creatures;
}

std::vector<dnd::Creature> parse_creatures_from_directory(
    const std::string& directory) {
  std::vector<dnd::Creature> all_creatures;
  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".json") {
      auto creatures = parse_creatures_from_file(entry.path().string());
      all_creatures.insert(all_creatures.end(), creatures.begin(),
                           creatures.end());
    }
  }

  return all_creatures;
}
