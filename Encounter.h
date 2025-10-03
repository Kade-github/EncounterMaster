#ifndef _ENCOUNTER_H
#define _ENCOUNTER_H

#include "Creature.h"

namespace dnd {
	class Encounter {
	 private:
		std::optional<std::vector<std::string>> creatures;
		std::optional<std::string> name;
        std::optional<std::vector<std::string>> notes;

	 public:
        std::vector<Creature> creature_objects;

		void add_creature(Creature creature) {
		  std::vector<std::string> get_creatr =
                      creatures.value_or(std::vector<std::string>());
          get_creatr.push_back(creature.to_string() + " (" +
                                     creature.original_list +
                                     ")");
          creature.id = creature_objects.size();
          creature_objects.push_back(creature);

		  set_creatures(get_creatr);
        }

		void remove_creature(const Creature& creature) {
		  std::vector<std::string> get_creatr =
					  creatures.value_or(std::vector<std::string>());
		  std::vector<std::string> get_notes =
                      notes.value_or(std::vector<std::string>());

		  auto obj_it = std::find_if(creature_objects.begin(), creature_objects.end(),
									 [&creature](const Creature& c) {
									   return c.to_string() == creature.to_string() &&
                               c.original_list == creature.original_list &&
                               creature.id == c.id;
                           });
          if (obj_it != creature_objects.end()) {
              int index = std::distance(creature_objects.begin(), obj_it);

			  creature_objects.erase(obj_it);
              if (index < get_notes.size()) {
                get_notes.erase(get_notes.begin() + index);
              }

			  get_creatr.erase(get_creatr.begin() + index);
          }

		  set_creatures(get_creatr);
          set_notes(get_notes);
        }

		std::vector<std::string> get_creatures() const {
			return creatures.value_or(std::vector<std::string>());
        }
		void set_creatures(std::optional<std::vector<std::string>> value) {
          this->creatures = value;
        }

		std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

		std::optional<std::vector<std::string>> get_notes() const {
          return notes;
        }
		void set_notes(std::optional<std::vector<std::string>> value) {
		  this->notes = value;
        }

		json to_json() const {
		  json j;
		  if (creatures.has_value()) {
			j["creatures"] = json::array();
            j["notes"] = json::array();
            int i = 0;
            for (const auto& creature : creatures.value()) {
              if (creature.find(" (" + std::to_string(i) + ")") == std::string::npos) {
                  j["creatures"].push_back(creature + " (" + std::to_string(i) + ")");
              } else {
                j["creatures"].push_back(creature);
              }
              auto creature_object = creature_objects[i];
              j["notes"].push_back(creature_object.get_notes().value_or(""));
              i++;
            }
		  }
		  if (name.has_value()) j["name"] = name.value();
		  return j;
        }
	};
}

dnd::Encounter parse_encounter_from_file(void* state, const std::string& filename);

void save_encounter_to_file(const dnd::Encounter& encounter,
                            const std::string& filename);
#endif