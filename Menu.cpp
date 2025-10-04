#include "Menu.h"

#include <SDL3_image/SDL_image.h>

#include "./imgui/imgui.h"
#include "./imgui/imgui_internal.h"
#include "imguifilediag/ImGuiFileDialog.h"

#ifndef VERSION
#define VERSION "Version 1.0.1"
#endif

#include <regex>

std::string _last_encounter_file = "";
bool _is_opening_file = false;

SDL_Texture* logo = nullptr;

void openEncounter(ToolState* state)
{
    _is_opening_file = true;
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                            ".encounter", config);
}

void saveAsEncounter(ToolState* state)
{
    _is_opening_file = false;
    IGFD::FileDialogConfig config;
    config.path = ".";
    config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
    ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save File",
                                            ".encounter", config);
}

bool edit_focused = false;

void saveEncounter(ToolState* state) {
  if (edit_focused)
  {
    saveCreature(state);
    return;
  }

  if (_last_encounter_file.empty()) {
    saveAsEncounter(state);
  } else {
    save_encounter_to_file(state->current_encounter, _last_encounter_file);
  }
}

void newEncounter(ToolState* state)
{
    state->current_encounter = dnd::Encounter();
}

bool show_about = false;

void menuDraw(ToolState* state) {
  if (logo == nullptr) {
    logo = loadTextureFromFile(state->renderer, "assets/logo_encounter.png");
  }

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
  static bool first_time = true;
  if (first_time) {
    first_time = false;

    // Remove any previous layout
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetIO().DisplaySize);

    // Split dockspace into middle and right
    ImGuiID dockMiddle, dockRight;
    dockRight = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.33f,
                                            nullptr, &dockMiddle);

    // Dock windows
    ImGui::DockBuilderDockWindow("Creature List", dockRight);
    ImGui::DockBuilderDockWindow("Creature Edit", dockRight);
    ImGui::DockBuilderDockWindow("Creature View", dockRight);
    ImGui::DockBuilderDockWindow("Battle", dockMiddle);
    ImGui::DockBuilderDockWindow("Encounter Edit", dockMiddle);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  if (ImGui::IsKeyChordPressed(ImGuiKey_N | ImGuiMod_Ctrl)) newEncounter(state);
  if (ImGui::IsKeyChordPressed(ImGuiKey_O | ImGuiMod_Ctrl))
    openEncounter(state);
  if (ImGui::IsKeyChordPressed(ImGuiKey_S | ImGuiMod_Ctrl))
    saveEncounter(state);
  if (ImGui::IsKeyChordPressed(ImGuiKey_S | ImGuiMod_Ctrl | ImGuiMod_Shift))
    saveAsEncounter(state);

  if (ImGui::IsKeyChordPressed(ImGuiKey_R | ImGuiMod_Ctrl)) {
    state->creatures = parse_creatures_from_directory("creatures");
    state->filtered_creatures.clear();
    state->creature_page = 0;
    state->creature_selected_index = -1;
    state->page.creatures.clear();
  }

  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New", "Ctrl+N")) {
      newEncounter(state);
    }
    if (ImGui::MenuItem("Open...", "Ctrl+O")) {
      openEncounter(state);
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {
      saveEncounter(state);
    }
    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
      saveAsEncounter(state);
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Creature"))
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
    if (ImGui::MenuItem("Reload Creatures", "Ctrl+R")) {
      state->creatures = parse_creatures_from_directory("creatures");
      state->filtered_creatures.clear();
      state->creature_page = 0;
      state->creature_selected_index = -1;
      state->page.creatures.clear();
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Help"))
  {
    if (ImGui::MenuItem("About")) {
      show_about = true;
    }
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();

  if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", 32 | ImGuiWindowFlags_NoDocking, ImVec2(320,320)) ||
      ImGuiFileDialog::Instance()->Display("SaveFileDlgKey",
                                           32 | ImGuiWindowFlags_NoDocking,
                                           ImVec2(320, 320))) {

    if (ImGuiFileDialog::Instance()->IsOk()) {  // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
      
      if (_is_opening_file) {
        state->current_encounter = parse_encounter_from_file(state, filePathName);
      } else {
        save_encounter_to_file(state->current_encounter, filePathName);
      }
      _last_encounter_file = filePathName;
    }

    ImGuiFileDialog::Instance()->Close();
  }

  ImGui::End();

  
  ImGui::SetNextWindowSizeConstraints(ImVec2(300, 500),
                                      ImVec2(FLT_MAX, FLT_MAX));
  ImGui::Begin("Creature List");
  ImGui::Text("Number of creatures: %zu", state->creatures.size());
  if (state->filtered_creatures.size() > 0) {
    ImGui::Text("Number of filtered creatures: %zu",
                state->filtered_creatures.size());
  }
  ImGui::Text("Filter:");
  ImGui::SameLine();
  if (ImGui::InputText("##Search", state->creature_filter, 256) ||
      state->reload_creatures)
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
      if (std::regex_search(
              creature.get_name().value() + " (" + creature.original_list + ")",
              filter_regex)) {
        state->filtered_creatures.push_back(creature);
      }
    }
    if (state->filtered_creatures.size() == state->creatures.size()) {
      state->filtered_creatures.clear();
    }
    state->page.creatures.clear();
  }
  displayCreatures(state);
  ImGui::End();

  if (state->reload_creatures) state->reload_creatures = false;

  ImGui::SetNextWindowSizeConstraints(ImVec2(300, 500),
                                      ImVec2(FLT_MAX, FLT_MAX));
  if (state->encounter_planner)
    ImGui::Begin("Creature Edit");
  else
    ImGui::Begin("Creature View");
  edit_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
  displayCreatureEdit(state);
  ImGui::End();

  
  ImGui::SetNextWindowSizeConstraints(ImVec2(300, 500),
                                      ImVec2(FLT_MAX, FLT_MAX));
  if (state->encounter_planner) {
      ImGui::Begin("Encounter Edit");
    displayEncounterEdit(state);
    ImGui::End();
  }

  if (show_about) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(500, 535), ImVec2(500, 535));
    ImGui::Begin("About EncounterMaster", &show_about,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("EncounterMaster");
    if (logo) {
      ImGui::SameLine(ImGui::GetWindowWidth() - 160);
      ImGui::Image((ImTextureID)(intptr_t)logo, ImVec2(134, 184.8));
    }
    ImGui::Text(VERSION);
    ImGui::Separator();

    ImGui::TextWrapped(
        "A tool for managing Dungeons & Dragons encounters, creating and "
        "editing creatures, and saving them for later use.");

    ImGui::Separator();

    ImGui::TextWrapped(
        "Creatures are downloaded from creatures_list.txt, which contains "
        "mirrors for creature data. You are encouraged to make your own lists with the save feature - and to host them as well.");

    ImGui::Separator();

    ImGui::TextWrapped("Created by Kade");

    ImGui::Separator();
    // Links

    ImGui::TextLinkOpenURL("Github",
                           "https://github.com/Kade-github/EncounterMaster");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("efortner (Default Creature List)",
                           "https://gist.github.com/efortner/");

    ImGui::End();
  }
}
