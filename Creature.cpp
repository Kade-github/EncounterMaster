#include "Creature.h"
#include <fstream>
#include <SDL3/SDL.h>

dnd::Creature parse_creature(const json& j) {
  dnd::Creature creature;
  
  try {
    creature.set_name(j.value("name", "Unknown Creature"));
    creature.set_ac(j.value("ac", 10));
    Size size;
    std::string size_str = j.value("size", "Medium");
    if (size_str == "tiny")
      size = Size::TINY;
    else if (size_str == "small")
      size = Size::SMALL;
    else if (size_str == "medium")
      size = Size::MEDIUM;
    else if (size_str == "large")
      size = Size::LARGE;
    else if (size_str == "huge")
      size = Size::HUGE;
    else if (size_str == "gargantuan")
      size = Size::GARGANTUAN;
    else
      size = Size::MEDIUM;  // Default to Medium if unrecognized

    creature.set_size(size);

    CreatureType type;
    std::string type_str = j.value("creatureType", "humanoid");
    if (type_str == "aberration")
      type = CreatureType::ABERRATION;
    else if (type_str == "beast")
      type = CreatureType::BEAST;
    else if (type_str == "celestial")
      type = CreatureType::CELESTIAL;
    else if (type_str == "construct")
      type = CreatureType::CONSTRUCT;
    else if (type_str == "dragon")
      type = CreatureType::DRAGON;
    else if (type_str == "elemental")
      type = CreatureType::ELEMENTAL;
    else if (type_str == "fey")
      type = CreatureType::FEY;
    else if (type_str == "fiend")
      type = CreatureType::FIEND;
    else if (type_str == "giant")
      type = CreatureType::GIANT;
    else if (type_str == "humanoid")
      type = CreatureType::HUMANOID;
    else if (type_str == "monstrosity")
      type = CreatureType::MONSTROSITY;
    else if (type_str == "ooze")
      type = CreatureType::OOZE;
    else if (type_str == "plant")
      type = CreatureType::PLANT;
    else if (type_str == "undead")
      type = CreatureType::UNDEAD;
    else
      type = CreatureType::HUMANOID;  // Default to Humanoid if unrecognized

    creature.set_creature_type(type);
    Alignment alignment;
    std::string alignment_str = j.value("alignment", "Unaligned");
    if (alignment_str == "lawful good")
      alignment = Alignment::LAWFUL_GOOD;
    else if (alignment_str == "neutral good")
      alignment = Alignment::NEUTRAL_GOOD;
    else if (alignment_str == "chaotic good")
      alignment = Alignment::CHAOTIC_GOOD;
    else if (alignment_str == "lawful neutral")
      alignment = Alignment::LAWFUL_NEUTRAL;
    else if (alignment_str == "true neutral")
      alignment = Alignment::TRUE_NEUTRAL;
    else if (alignment_str == "chaotic neutral")
      alignment = Alignment::CHAOTIC_NEUTRAL;
    else if (alignment_str == "lawful evil")
      alignment = Alignment::LAWFUL_EVIL;
    else if (alignment_str == "neutral evil")
      alignment = Alignment::NEUTRAL_EVIL;
    else if (alignment_str == "chaotic evil")
      alignment = Alignment::CHAOTIC_EVIL;
    else if (alignment_str == "any alignment")
      alignment = Alignment::ANY;
    else if (alignment_str == "any evil alignment")
      alignment = Alignment::ANY_EVIL_ALIGNMENT;
    else
      alignment =
          Alignment::AL_UNALIGNED;  // Default to Unaligned if unrecognized

    creature.set_alignment(alignment);
  }
  catch (const std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error parsing creature basic info: %s",
                 e.what());

  }

  try {
    if (j.contains("languages") && j["languages"].is_array()) {
      std::vector<std::string> languages =
          j["languages"].get<std::vector<std::string>>();
      creature.set_languages(languages);
    }

    creature.set_hit_dice(j.value("hitDice", "1d6"));

    if (j.contains("speed") && j["speed"].is_object()) {
      Speed speed;
      if (j["speed"].contains("walk"))
        speed.set_walk(j["speed"].value("walk", 0));
      if (j["speed"].contains("fly")) speed.set_fly(j["speed"].value("fly", 0));
      if (j["speed"].contains("swim"))
        speed.set_swim(j["speed"].value("swim", 0));
      if (j["speed"].contains("burrow"))
        speed.set_burrow(j["speed"].value("burrow", 0));
      if (j["speed"].contains("climb"))
        speed.set_climb(j["speed"].value("climb", 0));
      if (j["speed"].contains("hover"))
        speed.set_hover(j["speed"].value("hover", false));
      creature.set_speed(speed);
    }

    if (j.contains("modifiers") && j["modifiers"].is_object()) {
      Modifiers modifiers;
      if (j["modifiers"].contains("str"))
        modifiers.set_str(j["modifiers"].value("str", 10));
      if (j["modifiers"].contains("dex"))
        modifiers.set_dex(j["modifiers"].value("dex", 10));
      if (j["modifiers"].contains("con"))
        modifiers.set_con(j["modifiers"].value("con", 10));
      if (j["modifiers"].contains("int"))
        modifiers.set_modifiers_int(j["modifiers"].value("int", 10));
      if (j["modifiers"].contains("wis"))
        modifiers.set_wis(j["modifiers"].value("wis", 10));
      if (j["modifiers"].contains("cha"))
        modifiers.set_cha(j["modifiers"].value("cha", 10));

      creature.set_modifiers(modifiers);
    }

    if (j.contains("stats") && j["stats"].is_object()) {
      Modifiers stats;
      if (j["stats"].contains("str"))
        stats.set_str(j["stats"].value("str", 10));
      if (j["stats"].contains("dex"))
        stats.set_dex(j["stats"].value("dex", 10));
      if (j["stats"].contains("con"))
        stats.set_con(j["stats"].value("con", 10));
      if (j["stats"].contains("int"))
        stats.set_modifiers_int(j["stats"].value("int", 10));
      if (j["stats"].contains("wis"))
        stats.set_wis(j["stats"].value("wis", 10));
      if (j["stats"].contains("cha"))
        stats.set_cha(j["stats"].value("cha", 10));
      creature.set_stats(stats);
    }

    if (j.contains("savingThrows") && j["savingThrows"].is_object()) {
      Modifiers saving_throws;
      if (j["savingThrows"].contains("str"))
        saving_throws.set_str(j["savingThrows"].value("str", 0));
      if (j["savingThrows"].contains("dex"))
        saving_throws.set_dex(j["savingThrows"].value("dex", 0));
      if (j["savingThrows"].contains("con"))
        saving_throws.set_con(j["savingThrows"].value("con", 0));
      if (j["savingThrows"].contains("int"))
        saving_throws.set_modifiers_int(j["savingThrows"].value("int", 0));
      if (j["savingThrows"].contains("wis"))
        saving_throws.set_wis(j["savingThrows"].value("wis", 0));
      if (j["savingThrows"].contains("cha"))
        saving_throws.set_cha(j["savingThrows"].value("cha", 0));
      creature.set_saving_throws(saving_throws);
    }

    if (j.contains("skills") && j["skills"].is_object()) {
      std::map<std::string, int> skills;
      for (auto& [key, value] : j["skills"].items()) {
        skills[key] = value.get<int>();
      }
      creature.set_skills(skills);
    }

    if (j.contains("traits") && j["traits"].is_array()) {
      std::vector<std::string> traits =
          j["traits"].get<std::vector<std::string>>();
      creature.set_traits(traits);
    }
  }
  catch (const std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error parsing creature modifiers/stats: %s", e.what());

  }

  try {
    if (j.contains("actions") && j["actions"].is_object()) {
      Actions actions;
      if (j["actions"].contains("attackRolls") &&
          j["actions"]["attackRolls"].is_array()) {
        std::vector<AttackRoll> attack_rolls;
        for (const auto& ar : j["actions"]["attackRolls"]) {
          AttackRoll attack_roll;
          if (ar.contains("name")) attack_roll.set_name(ar.value("name", ""));
          if (ar.contains("attackType")) {
            std::string at_str = ar.value("attackType", "Melee");
            // to lower
            std::transform(at_str.begin(), at_str.end(), at_str.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            AttackType at;
            if (at_str == "melee")
              at = AttackType::MELEE;
            else if (at_str == "ranged")
              at = AttackType::RANGED;
            else if (at_str == "magic")
              at = AttackType::MAGIC;
            else if (at_str == "meleespell")
              at = AttackType::MELEE_SPELL;
            else if (at_str == "rangedspell")
              at = AttackType::RANGED_SPELL;
            else if (at_str == "meleeweapon")
              at = AttackType::MELEE_WEAPON;
            else if (at_str == "rangedweapon")
              at = AttackType::RANGED_WEAPON;
            else
              at = AttackType::MELEE;  // Default to Melee if unrecognized
            attack_roll.set_attack_type(at);
          }
          if (ar.contains("reach")) attack_roll.set_reach(ar.value("reach", 0));
          if (ar.contains("hit")) attack_roll.set_hit(ar.value("hit", 0));
          if (ar.contains("damage") && ar["damage"].is_array()) {
            std::vector<Damage> damages;
            for (const auto& dmg : ar["damage"]) {
              Damage damage;
              if (dmg.contains("roll")) damage.set_roll(dmg.value("roll", ""));
              if (dmg.contains("damageType")) {
                std::string dt_str = dmg.value("damageType", "slashing");
                DamageType dt;
                if (dt_str == "acid")
                  dt = DamageType::ACID;
                else if (dt_str == "bludgeoning")
                  dt = DamageType::BLUDGEONING;
                else if (dt_str == "cold")
                  dt = DamageType::COLD;
                else if (dt_str == "fire")
                  dt = DamageType::FIRE;
                else if (dt_str == "force")
                  dt = DamageType::FORCE;
                else if (dt_str == "lightning")
                  dt = DamageType::LIGHTNING;
                else if (dt_str == "necrotic")
                  dt = DamageType::NECROTIC;
                else if (dt_str == "piercing")
                  dt = DamageType::PIERCING;
                else if (dt_str == "poison")
                  dt = DamageType::POISON;
                else if (dt_str == "psychic")
                  dt = DamageType::PSYCHIC;
                else if (dt_str == "radiant")
                  dt = DamageType::RADIANT;
                else if (dt_str == "slashing")
                  dt = DamageType::SLASHING;
                else if (dt_str == "thunder")
                  dt = DamageType::THUNDER;
                else
                  dt = DamageType::SLASHING;  // Default to Slashing if
                                              // unrecognized
                damage.set_damage_type(dt);
              }
              if (dmg.contains("save")) {
                Save save;
                if (dmg["save"].contains("type"))
                  save.set_type(dmg["save"].value("type", ""));
                if (dmg["save"].contains("saveDC"))
                  save.set_save_dc(dmg["save"].value("save_dc", 0));
                if (dmg["save"].contains("dc"))
                  save.set_dc(dmg["save"].value("dc", 0));
                damage.set_save(save);
              } else if (dmg.contains("saveDC")) {
                Save& s = damage.get_save().value_or(Save());
                s.set_save_dc(dmg.value("saveDC", 0));
              } else if (dmg.contains("dc")) {
                Save& s = damage.get_save().value_or(Save());
                s.set_dc(dmg.value("dc", 0));
              }
              damages.push_back(damage);
            }
          }
          attack_rolls.push_back(attack_roll);
        }
        actions.set_attack_rolls(attack_rolls);
      }
      if (j["actions"].contains("list")) {
        std::vector<std::string> action_list =
            j["actions"]["list"].get<std::vector<std::string>>();
        actions.set_list(action_list);
      }
      creature.set_actions(actions);
    }

    if (j.contains("legendaryActions") && j["legendaryActions"].is_array()) {
      std::vector<std::string> legendary_actions =
          j["legendaryActions"].get<std::vector<std::string>>();
      creature.set_legendary_actions(legendary_actions);
    }

    if (j.contains("reactions") && j["reactions"].is_array()) {
      std::vector<std::string> reactions =
          j["reactions"].get<std::vector<std::string>>();
      creature.set_reactions(reactions);
    }
  }
  catch (const std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error parsing creature actions/attacks: %s", e.what());

  }
  try {
    if (j.contains("challenge") && j["challenge"].is_object()) {
      Challenge challenge;
      if (j["challenge"].contains("rating"))
        challenge.set_rating(j["challenge"].value("rating", "0"));
      if (j["challenge"].contains("xp"))
        challenge.set_xp(j["challenge"].value("xp", 0));
      creature.set_challenge(challenge);
    }

    if (j.contains("imageUrl")) creature.set_image_url(j.value("imageUrl", ""));
  }
  catch (const std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Error parsing creature challenge/image: %s", e.what());

  }

  try {
    if (j.contains("notes")) creature.set_notes(j.value("notes", ""));
  }
  catch (const std::exception& e) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error parsing creature notes: %s",
                 e.what());
  }

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
  std::string filename_no_ext = std::filesystem::path(filename).stem().string();
  for (const auto& item : j) {
    dnd::Creature creature;
    try {
        creature = parse_creature(item);
    }
    catch (const std::exception& e) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Error parsing creature in file %s: %s", filename.c_str(),
                   e.what());
      continue;
    }

    creature.original_list = filename_no_ext;
    creatures.push_back(creature);
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