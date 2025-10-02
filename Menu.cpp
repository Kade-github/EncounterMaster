#include "Menu.h"

#include "imgui/imgui.h"

void menuDraw(ToolState* state) {
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("EncounterMaster", nullptr,
               ImGuiWindowFlags_MenuBar |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New", "Ctrl+N")) {}
    if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();
  ImGui::End();
}
