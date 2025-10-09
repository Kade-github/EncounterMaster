#ifndef CREATURE_H
#define CREATURE_H

#include <nlohmann/json.hpp>

#include <optional>
#include <regex>
#include <stdexcept>

using json = nlohmann::json;


enum class AttackType : int {
  MAGIC = 0,
  MELEE = 1,
  MELEE_SPELL = 2,
  MELEE_WEAPON = 3,
  RANGED = 4,
  RANGED_SPELL = 5,
  RANGED_WEAPON = 6
};

enum class DamageType : int {
  ACID = 0,
  BLUDGEONING = 1,
  COLD = 2,
  FIRE = 3,
  FORCE = 4,
  LIGHTNING = 5,
  NECROTIC = 6,
  PIERCING = 7,
  POISON = 8,
  PSYCHIC = 9,
  RADIANT = 10,
  SLASHING = 11,
  THUNDER = 12
};

class Save {
 public:
  Save() = default;
  virtual ~Save() = default;

 private:
  std::optional<std::string> type;
  std::optional<int> save_dc;
  std::optional<int> dc;

 public:
  Save(std::string type, int save_dc, int dc)
      : type(type), save_dc(save_dc), dc(dc) {}
  std::optional<std::string> get_type() const { return type; }
  void set_type(std::optional<std::string> value) { this->type = value; }

  std::optional<int> get_save_dc() const { return save_dc; }
  void set_save_dc(std::optional<int> value) { this->save_dc = value; }

  std::optional<int> get_dc() const { return dc; }
  void set_dc(std::optional<int> value) { this->dc = value; }

  json to_json() const {
    json j;
    if (type.has_value()) j["type"] = type.value();
    if (save_dc.has_value()) j["save_dc"] = save_dc.value();
    if (dc.has_value()) j["dc"] = dc.value();
    return j;
  }
};

class Damage {
 public:
  Damage() = default;
  virtual ~Damage() = default;

 private:
  std::optional<std::string> roll;
  std::optional<DamageType> damage_type;
  std::optional<std::string> condition;
  std::optional<std::string> save_type;
  std::optional<int> save_dc;
  std::optional<Save> save;

 public:
  Damage(std::string roll, DamageType damage_type, std::string condition,
         std::string save_type, int save_dc, Save save)
      : roll(roll),
        damage_type(damage_type),
        condition(condition),
        save_type(save_type),
        save_dc(save_dc),
        save(save) {}
  std::optional<std::string> get_roll() const { return roll; }
  void set_roll(std::optional<std::string> value) { this->roll = value; }

  std::optional<DamageType> get_damage_type() const { return damage_type; }
  void set_damage_type(std::optional<DamageType> value) {
    this->damage_type = value;
  }

  std::optional<std::string> get_condition() const { return condition; }
  void set_condition(std::optional<std::string> value) {
    this->condition = value;
  }

  std::optional<std::string> get_save_type() const { return save_type; }
  void set_save_type(std::optional<std::string> value) {
    this->save_type = value;
  }

  std::optional<int> get_save_dc() const { return save_dc; }
  void set_save_dc(std::optional<int> value) { this->save_dc = value; }

  std::optional<Save> get_save() const { return save; }
  void set_save(std::optional<Save> value) { this->save = value; }

  json to_json() const {
    json j;
    if (roll.has_value()) j["roll"] = roll.value();
    if (damage_type.has_value())
      j["damage_type"] = static_cast<int>(damage_type.value());
    if (condition.has_value()) j["condition"] = condition.value();
    if (save_type.has_value()) j["save_type"] = save_type.value();
    if (save_dc.has_value()) j["save_dc"] = save_dc.value();
    if (save.has_value()) j["save"] = save.value().to_json();
    return j;
  }
};

class AttackRoll {
 public:
  AttackRoll() = default;
  virtual ~AttackRoll() = default;

 private:
  std::optional<std::string> name;
  std::optional<AttackType> attack_type;
  std::optional<int> reach;
  std::optional<int> hit;
  std::optional<std::vector<Damage>> damage;

 public:
  AttackRoll(std::string name, AttackType attack_type, int reach,
             int hit, std::vector<Damage> damage)
      : name(name),
        attack_type(attack_type),
        reach(reach),
        hit(hit),
        damage(damage) {}
  std::optional<std::string> get_name() const { return name; }
  void set_name(std::optional<std::string> value) { this->name = value; }

  std::optional<AttackType> get_attack_type() const { return attack_type; }
  void set_attack_type(std::optional<AttackType> value) {
    this->attack_type = value;
  }

  std::optional<int> get_reach() const { return reach; }
  void set_reach(std::optional<int> value) { this->reach = value; }

  std::optional<int> get_hit() const { return hit; }
  void set_hit(std::optional<int> value) { this->hit = value; }

  std::optional<std::vector<Damage>> get_damage() const { return damage; }
  void set_damage(std::optional<std::vector<Damage>> value) {
    this->damage = value;
  }

  json to_json() const {
    json j;
    if (name.has_value()) j["name"] = name.value();
    if (attack_type.has_value())
      j["attack_type"] = static_cast<int>(attack_type.value());
    if (reach.has_value()) j["reach"] = reach.value();
    if (hit.has_value()) j["hit"] = hit.value();
    if (damage.has_value()) {
      j["damage"] = json::array();
      for (const auto& dmg : damage.value()) {
        j["damage"].push_back(dmg.to_json());
      }
    }
    return j;
  }
};

class Actions {
 public:
  Actions() = default;
  virtual ~Actions() = default;

 private:
  std::optional<std::vector<std::string>> list;
  std::optional<std::vector<AttackRoll>> attack_rolls;

 public:

  Actions(std::vector<std::string> list,
          std::vector<AttackRoll> attack_rolls)
      : list(list), attack_rolls(attack_rolls) {}
  std::optional<std::vector<std::string>> get_list() const { return list; }
  void set_list(std::optional<std::vector<std::string>> value) {
    this->list = value;
  }

  std::optional<std::vector<AttackRoll>> get_attack_rolls() const {
    return attack_rolls;
  }
  void set_attack_rolls(std::optional<std::vector<AttackRoll>> value) {
    this->attack_rolls = value;
  }

  json to_json() const {
    json j;
    if (list.has_value()) j["list"] = list.value();
    if (attack_rolls.has_value()) {
      j["attack_rolls"] = json::array();
      for (const auto& attack : attack_rolls.value()) {
        j["attack_rolls"].push_back(attack.to_json());
      }
    }

    return j;
  }
};

enum class Alignment : int {
  ANY = 0,
  ANY_EVIL_ALIGNMENT = 1,
  CHAOTIC_EVIL = 2,
  CHAOTIC_GOOD = 3,
  CHAOTIC_NEUTRAL = 4,
  LAWFUL_EVIL = 5,
  LAWFUL_GOOD = 6,
  LAWFUL_NEUTRAL = 7,
  TRUE_NEUTRAL = 8,
  NEUTRAL = 9,
  NEUTRAL_EVIL = 10,
  NEUTRAL_GOOD = 11,
  AL_UNALIGNED = 12
};

class Challenge {
 public:
  Challenge() = default;
  virtual ~Challenge() = default;

 private:
  std::optional<std::string> rating;
  std::optional<int> xp;

 public:
  Challenge(std::string rating, int xp) : rating(rating), xp(xp) {}
  std::optional<std::string> get_rating() const { return rating; }
  void set_rating(std::optional<std::string> value) { this->rating = value; }

  std::optional<int> get_xp() const { return xp; }
  void set_xp(std::optional<int> value) { this->xp = value; }

  json to_json() const {
    json j;
    if (rating.has_value()) j["rating"] = rating.value();
    if (xp.has_value()) j["xp"] = xp.value();
    return j;
  }
};

enum class CreatureType : int {
  ABERRATION = 0,
  BEAST = 1,
  CELESTIAL = 2,
  CONSTRUCT = 3,
  DRAGON = 4,
  ELEMENTAL = 5,
  FEY = 6,
  FIEND = 7,
  GIANT = 8,
  HUMANOID = 9,
  MONSTROSITY = 10,
  OOZE = 11,
  PLANT = 12,
  UNDEAD = 13
};

class Modifiers {
 public:
  Modifiers() = default;
  virtual ~Modifiers() = default;

 private:
  std::optional<int> str;
  std::optional<int> dex;
  std::optional<int> con;
  std::optional<int> modifiers_int;
  std::optional<int> wis;
  std::optional<int> cha;

 public:
  Modifiers(int str, int dex, int con, int modifiers_int, int wis, int cha)
      : str(str),
        dex(dex),
        con(con),
        modifiers_int(modifiers_int),
        wis(wis),
        cha(cha) {}

  std::optional<int> get_str() const { return str; }
  void set_str(std::optional<int> value) { this->str = value; }

  std::optional<int> get_dex() const { return dex; }
  void set_dex(std::optional<int> value) { this->dex = value; }

  std::optional<int> get_con() const { return con; }
  void set_con(std::optional<int> value) { this->con = value; }

  std::optional<int> get_modifiers_int() const { return modifiers_int; }
  void set_modifiers_int(std::optional<int> value) {
    this->modifiers_int = value;
  }

  std::optional<int> get_wis() const { return wis; }
  void set_wis(std::optional<int> value) { this->wis = value; }

  std::optional<int> get_cha() const { return cha; }
  void set_cha(std::optional<int> value) { this->cha = value; }

  Modifiers getMods() const {
    return Modifiers(std::round(((float)(get_str().value()) - 10.0f) / 2.0f), 
                     std::round(((float)(get_dex().value()) - 10.0f) / 2.0f), 
                     std::round(((float)(get_con().value()) - 10.0f) / 2.0f), 
                     std::round(((float)(get_modifiers_int().value()) - 10.0f) / 2.0f), 
                     std::round(((float)(get_wis().value()) - 10.0f) / 2.0f), 
                     std::round(((float)(get_cha().value()) - 10.0f) / 2.0f));
  }

  json to_json() const {
    json j;
    if (str.has_value()) j["str"] = str.value();
    if (dex.has_value()) j["dex"] = dex.value();
    if (con.has_value()) j["con"] = con.value();
    if (modifiers_int.has_value()) j["int"] = modifiers_int.value();
    if (wis.has_value()) j["wis"] = wis.value();
    if (cha.has_value()) j["cha"] = cha.value();
    return j;
  }
};

enum class Size : int { GARGANTUAN = 0, S_HUGE = 1, LARGE = 2, MEDIUM = 3, SMALL = 4, TINY = 5 };

class Speed {
 public:
  Speed() = default;
  virtual ~Speed() = default;

 private:
  std::optional<int> walk;
  std::optional<int> fly;
  std::optional<int> swim;
  std::optional<int> burrow;
  std::optional<int> climb;
  std::optional<bool> hover;

 public:
   Speed(int walk, int fly, int swim, int burrow, int climb, bool hover)
      : walk(walk),
        fly(fly),
        swim(swim),
        burrow(burrow),
        climb(climb),
        hover(hover) {}

  std::optional<int> get_walk() const { return walk; }
  void set_walk(std::optional<int> value) { this->walk = value; }

  std::optional<int> get_fly() const { return fly; }
  void set_fly(std::optional<int> value) { this->fly = value; }

  std::optional<int> get_swim() const { return swim; }
  void set_swim(std::optional<int> value) { this->swim = value; }

  std::optional<int> get_burrow() const { return burrow; }
  void set_burrow(std::optional<int> value) { this->burrow = value; }

  std::optional<int> get_climb() const { return climb; }
  void set_climb(std::optional<int> value) { this->climb = value; }

  std::optional<bool> get_hover() const { return hover; }
  void set_hover(std::optional<bool> value) { this->hover = value; }

  json to_json() const {
    json j;
    if (walk.has_value()) j["walk"] = walk.value();
    if (fly.has_value()) j["fly"] = fly.value();
    if (swim.has_value()) j["swim"] = swim.value();
    if (burrow.has_value()) j["burrow"] = burrow.value();
    if (climb.has_value()) j["climb"] = climb.value();
    if (hover.has_value()) j["hover"] = hover.value();
    return j;
  }
};

class CreatureElement {
 public:
  CreatureElement() = default;
  virtual ~CreatureElement() = default;

 private:
  std::optional<std::string> name;
  std::optional<int> ac;
  std::optional<Size> size;
  std::optional<CreatureType> creature_type;
  std::optional<Alignment> alignment;
  std::optional<std::vector<std::string>> languages;
  std::optional<std::string> hit_dice;
  std::optional<Speed> speed;
  std::optional<Modifiers> modifiers;
  std::optional<Modifiers> stats;
  std::optional<Modifiers> saving_throws;
  std::optional<std::map<std::string, int>> skills;
  std::optional<std::vector<std::string>> traits;
  std::optional<Actions> actions;
  std::optional<std::vector<std::string>> legendary_actions;
  std::optional<std::vector<std::string>> reactions;
  std::optional<Challenge> challenge;
  std::optional<std::string> image_url;
  std::optional<std::string> notes;

 public:
  int current_hit_points = 0;
  int temp_hit_points = 0;

  int id = 0;

  std::string type_to_string()
  { 
    std::string type = "";
    switch (creature_type.value_or(CreatureType::HUMANOID)) {
      case CreatureType::ABERRATION: type = "Aberration"; break;
      case CreatureType::BEAST: type = "Beast"; break;
      case CreatureType::CELESTIAL: type = "Celestial"; break;
      case CreatureType::CONSTRUCT: type = "Construct"; break;
      case CreatureType::DRAGON: type = "Dragon"; break;
      case CreatureType::ELEMENTAL: type = "Elemental"; break;
      case CreatureType::FEY: type = "Fey"; break;
      case CreatureType::FIEND: type = "Fiend"; break;
      case CreatureType::GIANT: type = "Giant"; break;
      case CreatureType::HUMANOID: type = "Humanoid"; break;
      case CreatureType::MONSTROSITY: type = "Monstrosity"; break;
      case CreatureType::OOZE: type = "Ooze"; break;
      case CreatureType::PLANT: type = "Plant"; break;
      case CreatureType::UNDEAD: type = "Undead"; break;
      default: type = "Unknown"; break;
    }
    return type;
  }

  std::string original_list = "";

  std::optional<std::string> get_name() const { return name; }
  void set_name(std::optional<std::string> value) { this->name = value; }

  std::optional<int> get_ac() const { return ac; }
  void set_ac(std::optional<int> value) { this->ac = value; }

  std::optional<Size> get_size() const { return size; }
  void set_size(std::optional<Size> value) { this->size = value; }

  std::optional<CreatureType> get_creature_type() const {
    return creature_type;
  }
  void set_creature_type(std::optional<CreatureType> value) {
    this->creature_type = value;
  }

  std::optional<Alignment> get_alignment() const { return alignment; }
  void set_alignment(std::optional<Alignment> value) {
    this->alignment = value;
  }

  std::optional<std::vector<std::string>> get_languages() const {
    return languages;
  }
  void set_languages(std::optional<std::vector<std::string>> value) {
    this->languages = value;
  }

  int get_max_hit_points() const {
    std::string getHitDice = hit_dice.value_or("0d0+0");
    int num_dice = 0;
    int die_size = 0;
    int modifier = 0;
    std::regex dice_regex(R"((\d+)d(\d+)([+-]\d+)?)");
    std::smatch match;
    if (std::regex_match(getHitDice, match, dice_regex)) {
      num_dice = std::stoi(match[1]);
      die_size = std::stoi(match[2]);
      if (match[3].matched) {
        modifier = std::stoi(match[3]);
      }
    } else {
      return 0;  // Invalid format
    }

    return (num_dice * die_size) + modifier;
  }

  std::optional<std::string> get_hit_dice() const { return hit_dice; }
  void set_hit_dice(std::optional<std::string> value) {
    this->hit_dice = value;
  }

  std::optional<Speed> get_speed() const { return speed; }
  void set_speed(std::optional<Speed> value) { this->speed = value; }

  std::optional<Modifiers> get_modifiers() const { return modifiers; }
  void set_modifiers(std::optional<Modifiers> value) {
    this->modifiers = value;
  }

  std::optional<Modifiers> get_stats() const { return stats; }
  void set_stats(std::optional<Modifiers> value) { this->stats = value; }

  std::optional<Modifiers> get_saving_throws() const { return saving_throws; }
  void set_saving_throws(std::optional<Modifiers> value) {
    this->saving_throws = value;
  }

  std::optional<std::map<std::string, int>> get_skills() const {
    return skills;
  }
  void set_skills(std::optional<std::map<std::string, int>> value) {
    this->skills = value;
  }

  std::optional<std::vector<std::string>> get_traits() const { return traits; }
  void set_traits(std::optional<std::vector<std::string>> value) {
    this->traits = value;
  }

  std::optional<Actions> get_actions() const { return actions; }
  void set_actions(std::optional<Actions> value) { this->actions = value; }

  std::optional<std::vector<std::string>> get_legendary_actions() const {
    return legendary_actions;
  }
  void set_legendary_actions(std::optional<std::vector<std::string>> value) {
    this->legendary_actions = value;
  }

  std::optional<std::vector<std::string>> get_reactions() const {
    return reactions;
  }
  void set_reactions(std::optional<std::vector<std::string>> value) {
    this->reactions = value;
  }

  std::optional<Challenge> get_challenge() const { return challenge; }
  void set_challenge(std::optional<Challenge> value) {
    this->challenge = value;
  }

  std::optional<std::string> get_image_url() const { return image_url; }
  void set_image_url(std::optional<std::string> value) {
    this->image_url = value;
  }

  std::optional<std::string> get_notes() const { return notes; }
  void set_notes(std::string value) { this->notes = value; }

  std::string to_string() const {
    return name.value_or("Unknown Creature"); 
  }

  json to_json() const {
    json j;
    if (name.has_value()) j["name"] = name.value();
    if (ac.has_value()) j["ac"] = ac.value();
    if (size.has_value()) {
      switch (size.value()) {
        case Size::GARGANTUAN: j["size"] = "gargantuan"; break;
        case Size::S_HUGE: j["size"] = "huge"; break;
        case Size::LARGE: j["size"] = "large"; break;
        case Size::MEDIUM: j["size"] = "medium"; break;
        case Size::SMALL: j["size"] = "small"; break;
        case Size::TINY: j["size"] = "tiny"; break;
      }
    }
    if (creature_type.has_value())
    {
        switch (creature_type.value()) {
            case CreatureType::ABERRATION: j["creatureType"] = "aberration"; break;
            case CreatureType::BEAST: j["creatureType"] = "beast"; break;
            case CreatureType::CELESTIAL: j["creatureType"] = "celestial"; break;
            case CreatureType::CONSTRUCT: j["creatureType"] = "construct"; break;
            case CreatureType::DRAGON: j["creatureType"] = "dragon"; break;
            case CreatureType::ELEMENTAL: j["creatureType"] = "elemental"; break;
            case CreatureType::FEY: j["creatureType"] = "fey"; break;
            case CreatureType::FIEND: j["creatureType"] = "fiend"; break;
            case CreatureType::GIANT: j["creatureType"] = "giant"; break;
            case CreatureType::HUMANOID: j["creatureType"] = "humanoid"; break;
            case CreatureType::MONSTROSITY: j["creatureType"] = "monstrosity"; break;
            case CreatureType::OOZE: j["creatureType"] = "ooze"; break;
            case CreatureType::PLANT: j["creatureType"] = "plant"; break;
            case CreatureType::UNDEAD: j["creatureType"] = "undead"; break;
        }
    }
    if (alignment.has_value())
    {
        switch (alignment.value())
        {
            case Alignment::ANY: j["alignment"] = "any"; break;
            case Alignment::ANY_EVIL_ALIGNMENT: j["alignment"] = "any evil alignment"; break;
            case Alignment::CHAOTIC_EVIL: j["alignment"] = "chaotic evil"; break;
            case Alignment::CHAOTIC_GOOD: j["alignment"] = "chaotic good"; break;
            case Alignment::CHAOTIC_NEUTRAL: j["alignment"] = "chaotic neutral"; break;
            case Alignment::LAWFUL_EVIL: j["alignment"] = "lawful evil"; break;
            case Alignment::LAWFUL_GOOD: j["alignment"] = "lawful good"; break;
            case Alignment::LAWFUL_NEUTRAL: j["alignment"] = "lawful neutral"; break;
            case Alignment::TRUE_NEUTRAL: j["alignment"] = "true neutral"; break;
            case Alignment::NEUTRAL: j["alignment"] = "neutral"; break;
            case Alignment::NEUTRAL_EVIL: j["alignment"] = "neutral evil"; break;
            case Alignment::NEUTRAL_GOOD: j["alignment"] = "neutral good"; break;
            case Alignment::AL_UNALIGNED:
              j["alignment"] = "unaligned";
              break;
        }
    }
    if (languages.has_value()) j["languages"] = languages.value();
    j["maxHitPoints"] = get_max_hit_points();
    if (hit_dice.has_value()) j["hitDice"] = hit_dice.value();
    if (speed.has_value()) j["speed"] = speed.value().to_json();
    if (modifiers.has_value())
      j["modifiers"] = stats.value().getMods().to_json();
    if (stats.has_value()) j["stats"] = stats.value().to_json();
    if (saving_throws.has_value())
      j["savingThrows"] = saving_throws.value().to_json();
    if (skills.has_value()) j["skills"] = skills.value();
    if (traits.has_value()) j["traits"] = traits.value();
    if (actions.has_value()) j["actions"] = actions.value().to_json();
    if (legendary_actions.has_value())
      j["legendaryActions"] = legendary_actions.value();
    if (reactions.has_value()) j["reactions"] = reactions.value();
    if (challenge.has_value())
      j["challenge"] = challenge.value().to_json();
    if (image_url.has_value()) j["imageUrl"] = image_url.value();
    if (notes.has_value()) j["notes"] = notes.value();
    return j;
  }
};



namespace dnd {
    using Creature = CreatureElement;

}


dnd::Creature parse_creature(const json& j);

std::vector<dnd::Creature> parse_creatures_from_file(const std::string& filename);

std::vector<dnd::Creature> parse_creatures_from_directory(
    const std::string& directory);

void save_creatures_to_file(const std::vector<dnd::Creature>& creatures,
                            const std::string& filename);

#endif