#include "Menu.h"
#include "imgui/imgui.h"

/*
    8 creatures per page
*/

void initCurrentPage(void* s, std::vector<dnd::Creature> all_creatures) {
  ToolState* state = static_cast<ToolState*>(s);
  if (state->page.creatures.size() == 0 || state->page.page_number != state->creature_page)
  {
    state->page.creatures.clear();
    // init it
    int index_start = state->creature_page * 8;
    int index_end = std::min(index_start + 8, static_cast<int>(all_creatures.size()));

    for (int i = index_start; i < index_end; i++) {
      state->page.creatures.push_back(all_creatures[i]);
    }
    state->page.page_number = state->creature_page;
  }
}

void displayCreatures(void* s) {
  ToolState* state = static_cast<ToolState*>(s);
  if (state->filtered_creatures.empty()) {
    // Show all creatures
    initCurrentPage(state, state->creatures);
  } else {
    // Show filtered creatures
    initCurrentPage(state, state->filtered_creatures);
  }

  int total_creatures = state->filtered_creatures.empty()
                            ? state->creatures.size()
                            : state->filtered_creatures.size();

  ImGui::BeginChild("##Creatures", ImVec2(0, 0), true);
  ImGui::Text("Page: %d/%d", state->creature_page + 1,
              (total_creatures + 7) / 8);
  ImGui::BeginDisabled(state->creature_page == 0);  // Disable if on first page
  if (ImGui::Button("Previous Page") && state->creature_page > 0) {
    state->creature_page--;
  }
  ImGui::EndDisabled();
  bool lastPage = (state->creature_page + 1) * 8 < total_creatures;
  ImGui::BeginDisabled(!lastPage);  // Disable if on last page
  ImGui::SameLine();
  if (ImGui::Button("Next Page") && lastPage) {
    state->creature_page++;
  }
  ImGui::EndDisabled();
  for (size_t i = 0; i < state->page.creatures.size(); i++) {
    ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)i), ImVec2(0, 0),
        ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);
    const auto& creature = state->page.creatures[i];
    ImGui::Text("%s (%s)", creature.get_name().value().c_str(),
                creature.original_list.c_str());
    ImGui::Text("AC: %d, HP: %d (%s)", creature.get_ac().value(),
                creature.get_max_hit_points(),
                creature.get_hit_dice().value().c_str());
    if (state->encounter_planner) {
      if (ImGui::Button("Edit")) {
        state->clear_image = true;
        state->current_creature = creature;
        // Focus on the edit tab
        ImGui::SetWindowFocus("Creature Edit");
      }
      ImGui::SameLine();
      ImGui::Button("Add");
    } else if (state->encounter_battler) {
      if (ImGui::Button("View")) {
        state->clear_image = true;
        state->current_creature = creature;
        // Focus on the view tab
        ImGui::SetWindowFocus("Creature View");
      }
    }
    if (i < state->page.creatures.size() - 1) {
      ImGui::Separator();
    }
    ImGui::EndChild();
  }
  ImGui::EndChild();
}
