#include "Menu.h"
#include "imgui/imgui.h"

#include <regex>

void menuDraw(ToolState* state) {
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  std::string title = "EncounterMaster";
  if (state->encounter_battler)
      title += " - Battler";
  else if (state->encounter_planner)
    title += " - Planner";

  ImGui::Begin(title.c_str(), nullptr,
               ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

  ImGuiID dockspace_id = ImGui::GetID("encounter_master_dockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New", "Ctrl+N")) {}
    if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Views")) {
    if (ImGui::MenuItem("Open Planner", "Ctrl+P")) {
      state->encounter_planner = true;
      state->encounter_battler = false;
    }
    if (ImGui::MenuItem("Open Battler", "Ctrl+B")) {
      state->encounter_battler = true;
      state->encounter_planner = false;
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Creature", state->encounter_planner))
  {
    if (ImGui::MenuItem("Download Creatures")) {
      state->is_downloading_creatures = true;
      state->should_download_creatures = true;
      downloadCreatures([state]() {
        state->is_downloading_creatures = false;
        state->should_download_creatures = false;
        state->creatures = parse_creatures_from_directory("creatures");
        if (state->creatures.empty()) {
          state->should_download_creatures = true;
        }
      });
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Create New", "Ctrl-Shift-N"))
    {

    }
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();
  ImGui::End();

  ImGui::Begin("Creature List");
  ImGui::Text("Number of creatures: %zu", state->creatures.size());
  if (state->filtered_creatures.size() > 0) {
    ImGui::Text("Number of filtered creatures: %zu",
                state->filtered_creatures.size());
  }
  ImGui::Text("Filter:");
  ImGui::SameLine();
  if (ImGui::InputText("##Search", state->creature_filter, 256))
  {
    state->filtered_creatures.clear();
    state->creature_page = 0;
    state->creature_selected_index = -1;
    // Filter creatures based on search
    state->filtered_creatures.clear();
    for (const auto& creature : state->creatures) {
      if (!creature.get_name().has_value()) continue;
      // Regex search, case insensitive

      std::regex filter_regex(state->creature_filter,
                              std::regex_constants::icase);
      if (std::regex_search(creature.get_name().value(), filter_regex)) {
        state->filtered_creatures.push_back(creature);
      }
    }
    state->page.creatures.clear();
  }
  displayCreatures(state);
  ImGui::End();

  if (state->encounter_planner) {
    ImGui::Begin("Creature Edit", nullptr);
    displayCreatureEdit(state);
    ImGui::End();
  }
}
