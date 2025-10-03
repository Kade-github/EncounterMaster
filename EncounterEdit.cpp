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
    int i = 0;
    for (auto& creature : state->current_encounter.creature_objects) {
      ImGui::Text("%s (%s)", creature.get_name().value().c_str(),
                  creature.original_list.c_str());
      ImGui::SameLine();
      if (ImGui::Button(("Remove##" + creature.to_string() + creature.original_list + std::to_string(i)).c_str())) {
        state->current_encounter.remove_creature(creature);
        break; // Break to avoid iterator invalidation
      }

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

      ImGui::Text("Actions");

      ImGui::Separator();

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