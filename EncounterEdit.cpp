#include "Menu.h"

#include "imgui/imgui.h"

void displayEncounterEdit(void* s)
{
  ToolState* state = (ToolState*)s;

  if (!state->encounter_planner) {
	ImGui::Text("Encounter editing is disabled in Battler mode.");
	return;
  }

  char nameBuffer[256];
  strncpy(
      nameBuffer,
      state->current_encounter.get_name().value_or("Encounter").c_str(),
      sizeof(nameBuffer));
  nameBuffer[sizeof(nameBuffer) - 1] = '\0';
  ImGui::InputText("Encounter Name", nameBuffer, sizeof(nameBuffer));
  state->current_encounter.set_name(std::string(nameBuffer));


  if (state->current_encounter.get_creatures().empty()) {
    ImGui::Text("No creatures in encounter.");
  } else {
    if (state->current_encounter.creature_objects.size() == 0)
    {
      ImGui::Text(
          "Failed to parse any creature names.");
      return;
    }

    // Encounter breakdown

    /// How many creatures

    std::map<std::string, int> creature_count;

    for (const auto& creature : state->current_encounter.creature_objects) {
      std::string name = creature.get_name().value().c_str();
      creature_count[name]++;
    }

    ImGui::Text("Encounter Creatures:");

    for (const auto& [name, count] : creature_count) {
      ImGui::Text("%s x%d", name.c_str(), count);
    }

    ImGui::Separator();

    int i = 0;
    for (auto& creature : state->current_encounter.creature_objects) {
      if (!ImGui::CollapsingHeader((creature.get_name().value() + " #" + std::to_string(i + 1) + " (" +
                                       creature.original_list + ")##" +
                                       std::to_string(i))
                                          .c_str())) {
        i++;
        continue;
      }
      ImGui::Text("%s (%s)", creature.get_name().value().c_str(),
                  creature.original_list.c_str());
      ImGui::SameLine();
      if (ImGui::Button(("Remove##" + creature.to_string() + creature.original_list + std::to_string(i)).c_str())) {
        state->current_encounter.remove_creature(creature);
        break; // Break to avoid iterator invalidation
      }

      ImGui::Text("AC: %d, HP: %d (%s)", creature.get_ac().value(),
                  creature.get_max_hit_points(),
                  creature.get_hit_dice().value_or("N/A").c_str());

      // Notes
      ImGui::Separator();

      ImGui::Text("Notes:");
      char notesBuffer[1024];
      strncpy(notesBuffer, creature.get_notes().value_or("").c_str(),
              sizeof(notesBuffer));
      notesBuffer[sizeof(notesBuffer) - 1] = '\0';
      ImGui::InputTextMultiline(
          ("##Notes" + creature.to_string() + creature.original_list +
           std::to_string(i))
              .c_str(),
          notesBuffer, sizeof(notesBuffer),
          ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
      creature.set_notes(std::string(notesBuffer));

      ImGui::Separator();

      ImGui::Text("Skills:");

      if (creature.get_skills().has_value()) {
        // Skills
        bool shownSkill = false;
        for (const auto& skill : creature.get_skills().value_or(std::map<std::string, int>())) {
          if (skill.second == 0) continue;
          ImGui::TextWrapped("%s: %d", skill.first.c_str(), skill.second);
          ImGui::Separator();
          shownSkill = true;
        }
        if (creature.get_skills()->empty() || !shownSkill) {
          ImGui::Text("No skills available.");
          ImGui::Separator();
        }
      } else {
        ImGui::Text("No skills available.");
        ImGui::Separator();
      }

      ImGui::Text("Traits:");

      if (creature.get_traits().has_value()) {
        // Traits
        for (const auto& trait : creature.get_traits().value_or(
                 std::vector<std::string>())) {
          ImGui::TextWrapped("%s", trait.c_str());
        ImGui::Separator();
        }
        if (creature.get_traits()->empty()) {
          ImGui::Text("No traits available.");
          ImGui::Separator();
        }
      } else {
        ImGui::Text("No traits available.");
        ImGui::Separator();
      }

      ImGui::Text("Actions:");

      if (creature.get_actions().has_value()) {
        // Actions

        for (const auto& action :
             creature.get_actions().value().get_list().value_or(
                 std::vector<std::string>())) {
          ImGui::TextWrapped("%s", action.c_str());
        ImGui::Separator();
        }
        if (creature.get_actions()->get_list()->empty()) {
          ImGui::Text("No actions available.");
          ImGui::Separator();
        }
      } else {
        ImGui::Text("No actions available.");
        ImGui::Separator();
      }

      ImGui::Text("Legendary Actions:");

      if (creature.get_legendary_actions().has_value()) {
        // Legendary Actions
        for (const auto& la : creature.get_legendary_actions().value_or(
                 std::vector<std::string>())) {
          ImGui::TextWrapped("%s", la.c_str());
        ImGui::Separator();
        }
        if (creature.get_legendary_actions()->empty()) {
          ImGui::Text("No legendary actions available.");
          ImGui::Separator();
        }
      } else {
        ImGui::Text("No legendary actions available.");
        ImGui::Separator();
      }

      ImGui::Text("Reactions:");

      if (creature.get_reactions().has_value()) {
        // Reactions
        for (const auto& reaction : creature.get_reactions().value_or(
                 std::vector<std::string>())) {
          ImGui::TextWrapped("%s", reaction.c_str());
        ImGui::Separator();
        }
        if (creature.get_reactions()->empty()) {
          ImGui::Text("No reactions available.");
          ImGui::Separator();
        }
      } else {
        ImGui::Text("No reactions available.");
        ImGui::Separator();
      }

      if (ImGui::Button(("View Details##" + std::to_string(i)).c_str()))
      {
        state->current_creature = creature;
        ImGui::SetWindowFocus("Creature Edit");
      }
      i++;
    }
  }
}