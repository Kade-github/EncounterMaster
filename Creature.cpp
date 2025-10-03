#include "Creature.h"
#include <fstream>
#include <SDL3/SDL.h>

dnd::Creature parse_creature(const json& j) {
  dnd::Creature creature;
  // Main fields
  creature.set_name(j.value("name", ""));
  creature.set_meta(j.value("meta", ""));

  // Stats
  creature.set_armor_class(j.value("Armor Class", ""));
  creature.set_hit_points(j.value("Hit Points", ""));
  creature.set_speed(j.value("Speed", ""));
  creature.set_str(j.value("STR", ""));
  creature.set_str_mod(j.value("STR_mod", ""));
  creature.set_dex(j.value("DEX", ""));
  creature.set_dex_mod(j.value("DEX_mod", ""));
  creature.set_con(j.value("CON", ""));
  creature.set_con_mod(j.value("CON_mod", ""));
  creature.set_creature_int(j.value("INT", ""));
  creature.set_int_mod(j.value("INT_mod", ""));
  creature.set_wis(j.value("WIS", ""));
  creature.set_wis_mod(j.value("WIS_mod", ""));
  creature.set_cha(j.value("CHA", ""));
  creature.set_cha_mod(j.value("CHA_mod", ""));

  // Other fields
  creature.set_saving_throws(j.value("Saving Throws", ""));

  creature.set_skills(j.value("Skills", ""));
  creature.set_senses(j.value("Senses", ""));
  creature.set_languages(j.value("Languages", ""));
  creature.set_challenge(j.value("Challenge", ""));

  // Actions and traits
  creature.set_traits(j.value("Traits", ""));
  creature.set_actions(j.value("Actions", ""));
  creature.set_legendary_actions(j.value("Legendary Actions", ""));

  // Image URL
  creature.set_img_url(j.value("img_url", ""));

  // Combat-related fields
  creature.set_damage_immunities(j.value("Damage Immunities", ""));
  creature.set_condition_immunities(j.value("Condition Immunities", ""));
  creature.set_damage_resistances(j.value("Damage Resistances", ""));
  creature.set_damage_vulnerabilities(j.value("Damage Vulnerabilities", ""));
  creature.set_reactions(j.value("Reactions", ""));

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
  SDL_Log("Loading creatures...");
  std::vector<dnd::Creature> all_creatures;
  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".json") {
      auto creatures = parse_creatures_from_file(entry.path().string());
      all_creatures.insert(all_creatures.end(), creatures.begin(),
                           creatures.end());
    }
  }

  save_creatures_to_file(all_creatures, "all_creatures.json");

  return all_creatures;
}

void save_creatures_to_file(const std::vector<dnd::Creature>& creatures,
                            const std::string& filename) {
  json j = json::array();
  for (dnd::Creature creature : creatures) {
    j.push_back(creature.to_json());
  }

  std::ofstream file(filename);
  file << j;
}