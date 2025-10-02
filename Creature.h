#ifndef CREATURE_H
#define CREATURE_H

#include <nlohmann/json.hpp>

#include <optional>
#include <regex>
#include <stdexcept>

class CreatureElement {
 public:
  CreatureElement() = default;
  virtual ~CreatureElement() = default;

 private:
  std::optional<std::string> name;
  std::optional<std::string> meta;
  std::optional<std::string> armor_class;
  std::optional<std::string> hit_points;
  std::optional<std::string> speed;
  std::optional<std::string> str;
  std::optional<std::string> str_mod;
  std::optional<std::string> dex;
  std::optional<std::string> dex_mod;
  std::optional<std::string> con;
  std::optional<std::string> con_mod;
  std::optional<std::string> creature_int;
  std::optional<std::string> int_mod;
  std::optional<std::string> wis;
  std::optional<std::string> wis_mod;
  std::optional<std::string> cha;
  std::optional<std::string> cha_mod;
  std::optional<std::string> saving_throws;
  std::optional<std::string> skills;
  std::optional<std::string> senses;
  std::optional<std::string> languages;
  std::optional<std::string> challenge;
  std::optional<std::string> traits;
  std::optional<std::string> actions;
  std::optional<std::string> legendary_actions;
  std::optional<std::string> img_url;
  std::optional<std::string> damage_immunities;
  std::optional<std::string> condition_immunities;
  std::optional<std::string> damage_resistances;
  std::optional<std::string> damage_vulnerabilities;
  std::optional<std::string> reactions;

 public:
  std::optional<std::string> get_name() const { return name; }
  void set_name(std::optional<std::string> value) { this->name = value; }

  std::optional<std::string> get_meta() const { return meta; }
  void set_meta(std::optional<std::string> value) { this->meta = value; }

  std::optional<std::string> get_armor_class() const { return armor_class; }
  void set_armor_class(std::optional<std::string> value) {
    this->armor_class = value;
  }

  std::optional<std::string> get_hit_points() const { return hit_points; }
  void set_hit_points(std::optional<std::string> value) {
    this->hit_points = value;
  }

  std::optional<std::string> get_speed() const { return speed; }
  void set_speed(std::optional<std::string> value) { this->speed = value; }

  std::optional<std::string> get_str() const { return str; }
  void set_str(std::optional<std::string> value) { this->str = value; }

  std::optional<std::string> get_str_mod() const { return str_mod; }
  void set_str_mod(std::optional<std::string> value) { this->str_mod = value; }

  std::optional<std::string> get_dex() const { return dex; }
  void set_dex(std::optional<std::string> value) { this->dex = value; }

  std::optional<std::string> get_dex_mod() const { return dex_mod; }
  void set_dex_mod(std::optional<std::string> value) { this->dex_mod = value; }

  std::optional<std::string> get_con() const { return con; }
  void set_con(std::optional<std::string> value) { this->con = value; }

  std::optional<std::string> get_con_mod() const { return con_mod; }
  void set_con_mod(std::optional<std::string> value) { this->con_mod = value; }

  std::optional<std::string> get_creature_int() const { return creature_int; }
  void set_creature_int(std::optional<std::string> value) {
    this->creature_int = value;
  }

  std::optional<std::string> get_int_mod() const { return int_mod; }
  void set_int_mod(std::optional<std::string> value) { this->int_mod = value; }

  std::optional<std::string> get_wis() const { return wis; }
  void set_wis(std::optional<std::string> value) { this->wis = value; }

  std::optional<std::string> get_wis_mod() const { return wis_mod; }
  void set_wis_mod(std::optional<std::string> value) { this->wis_mod = value; }

  std::optional<std::string> get_cha() const { return cha; }
  void set_cha(std::optional<std::string> value) { this->cha = value; }

  std::optional<std::string> get_cha_mod() const { return cha_mod; }
  void set_cha_mod(std::optional<std::string> value) { this->cha_mod = value; }

  std::optional<std::string> get_saving_throws() const { return saving_throws; }
  void set_saving_throws(std::optional<std::string> value) {
    this->saving_throws = value;
  }

  std::optional<std::string> get_skills() const { return skills; }
  void set_skills(std::optional<std::string> value) { this->skills = value; }

  std::optional<std::string> get_senses() const { return senses; }
  void set_senses(std::optional<std::string> value) { this->senses = value; }

  std::optional<std::string> get_languages() const { return languages; }
  void set_languages(std::optional<std::string> value) {
    this->languages = value;
  }

  std::optional<std::string> get_challenge() const { return challenge; }
  void set_challenge(std::optional<std::string> value) {
    this->challenge = value;
  }

  std::optional<std::string> get_traits() const { return traits; }
  void set_traits(std::optional<std::string> value) { this->traits = value; }

  std::optional<std::string> get_actions() const { return actions; }
  void set_actions(std::optional<std::string> value) { this->actions = value; }

  std::optional<std::string> get_legendary_actions() const {
    return legendary_actions;
  }
  void set_legendary_actions(std::optional<std::string> value) {
    this->legendary_actions = value;
  }

  std::optional<std::string> get_img_url() const { return img_url; }
  void set_img_url(std::optional<std::string> value) { this->img_url = value; }

  std::optional<std::string> get_damage_immunities() const {
    return damage_immunities;
  }
  void set_damage_immunities(std::optional<std::string> value) {
    this->damage_immunities = value;
  }

  std::optional<std::string> get_condition_immunities() const {
    return condition_immunities;
  }
  void set_condition_immunities(std::optional<std::string> value) {
    this->condition_immunities = value;
  }

  std::optional<std::string> get_damage_resistances() const {
    return damage_resistances;
  }
  void set_damage_resistances(std::optional<std::string> value) {
    this->damage_resistances = value;
  }

  std::optional<std::string> get_damage_vulnerabilities() const {
    return damage_vulnerabilities;
  }
  void set_damage_vulnerabilities(std::optional<std::string> value) {
    this->damage_vulnerabilities = value;
  }

  std::optional<std::string> get_reactions() const { return reactions; }
  void set_reactions(std::optional<std::string> value) {
    this->reactions = value;
  }
};

namespace dnd {
    using Creature = CreatureElement;
}

using json = nlohmann::json;

dnd::Creature parse_creature(const json& j);

std::vector<dnd::Creature> parse_creatures_from_file(const std::string& filename);

std::vector<dnd::Creature> parse_creatures_from_directory(
    const std::string& directory);

#endif