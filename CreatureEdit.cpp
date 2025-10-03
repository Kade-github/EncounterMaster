#include "Menu.h"
#include "imgui/imgui.h"

#include <SDL3_image/SDL_image.h>
#include <curl/curl.h>

#include <fstream>
#include <filesystem>
#include <string>

#include <filesystem>
#include <vector>

std::string image_data;
std::string image_filename = "downloaded_image.tmp";
CURL* creature_easy = nullptr;
CURLM* creature_multi = curl_multi_init();

std::vector<std::string> splitByDelimiter(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
        token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
        if (!token.empty()) {
            result.push_back(token);
        }
        start = end + 1;
        end = str.find(delimiter, start);
    }
    std::string token = str.substr(start);
    token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
    token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
    if (!token.empty()) {
        result.push_back(token);
    }
    return result;
}

void getBufferFromOptionalString(const std::optional<std::string>& optStr, char* buffer, size_t bufferSize) {
    if (optStr.has_value()) {
        strncpy(buffer, optStr->c_str(), bufferSize);
        buffer[bufferSize - 1] = '\0'; // Ensure null-termination
    } else {
        buffer[0] = '\0'; // Empty string if no value
    }
}

SDL_Texture* loadTextureFromFile(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_Load failed: %s",
                   SDL_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }
    return texture;
}

void displayAbilityScore(std::string name, int& score, bool sameLine = false,
                         bool modifier = true) {
  if (sameLine) ImGui::SameLine(200);
  ImGui::Text("%s:", name.c_str());
  ImGui::SameLine();
  ImGui::SetNextItemWidth(75);
  ImGui::InputInt(("##" + name).c_str(), &score);
  if (!modifier) return;
  ImGui::SameLine(); // modifier
  int mod = std::floor(((float)score - 10.0f) / 2.0f);
  ImGui::Text("%d", mod);
}

size_t curlWriteFunc(char* data, size_t size, size_t nmemb,
                     std::string* buffer) {
  size_t result = 0;

  if (buffer != NULL) {
    buffer->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}

void startImageDownload(const std::string& url) {
  creature_easy = curl_easy_init();
  if (!creature_easy) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "curl_easy_init failed");
    return;
  }

  image_filename = url.substr(url.find_last_of("/\\") + 1);

  SDL_Log("Starting image download from URL: %s", url.c_str());

  curl_easy_setopt(creature_easy, CURLOPT_URL, url.c_str());
  curl_easy_setopt(creature_easy, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(creature_easy, CURLOPT_WRITEDATA, &image_data);
  curl_easy_setopt(creature_easy, CURLOPT_WRITEFUNCTION, curlWriteFunc);
  curl_multi_add_handle(creature_multi, creature_easy);
}

void imageCallbacks(void* s)
{
  ToolState* state = (ToolState*)s;
  int still_running;
  CURLMcode mc = curl_multi_perform(creature_multi, &still_running);

  if (mc != CURLM_OK) {
    SDL_Log("curl_multi_perform() failed, code %d.", mc);
  }

  if (!still_running) {
    SDL_Log("Image download completed.");

    std::ofstream outFile("images/" + image_filename, std::ios::binary);
    outFile.write(image_data.data(), image_data.size());
    outFile.close();

    state->current_creature_texture =
        loadTextureFromFile(state->renderer, "images/" + image_filename);

    state->is_downloading_image = false;
    if (state->current_creature_texture == nullptr) state->image_error = true;

    // Download complete
    curl_multi_remove_handle(creature_multi, creature_easy);
    curl_easy_cleanup(creature_easy);
    creature_easy = nullptr;

    image_data.clear();
  }

}

void checkImage(ToolState* state)
{
  std::string url = state->current_creature.get_image_url().value_or("");
  if (url.find("http://") == 0 || url.find("https://") == 0) {
    std::filesystem::create_directories("images");
    // Check if the image is already downloaded
    if (std::filesystem::exists("images/" +
                                url.substr(url.find_last_of("/\\") + 1))) {
      SDL_Log("Loading cached image from URL: %s", url.c_str());
      state->current_creature_texture = loadTextureFromFile(
          state->renderer, "images/" + url.substr(url.find_last_of("/\\") + 1));
    }
    if (!state->current_creature_texture) {
      state->is_downloading_image = true;
      startImageDownload(url);
    }
  } else {
    SDL_Log("Loading local image from URL: %s", url.c_str());
    SDL_Texture* texture = loadTextureFromFile(state->renderer, url);
    if (texture) state->current_creature_texture = texture;
  }
}

void displayCreatureEdit(void* s) {
  ToolState* state = (ToolState*)s;
  if (!state->current_creature.get_name().has_value()) {
    ImGui::Text("No creature selected");
    return;
  }

  if (state->clear_image)
  {
    if (state->current_creature_texture) {
      SDL_DestroyTexture(state->current_creature_texture);
      state->current_creature_texture = nullptr;
    }
    state->clear_image = false;
    state->is_downloading_image = false;
    state->image_error = false;
    image_data.clear();
    if (creature_easy) {
      curl_multi_remove_handle(creature_multi, creature_easy);
      curl_easy_cleanup(creature_easy);
      creature_easy = nullptr;
    }
  }

  if (state->current_creature_texture == nullptr &&
      state->current_creature.get_image_url().has_value() &&
      !state->is_downloading_image && !state->image_error)
  {
    checkImage(state);
  }

  ImGui::BeginChild("##CreatureEdit", ImVec2(0, 0));
  char nameBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_name(), nameBuffer,
                              sizeof(nameBuffer));
  ImGui::Text("Name:");
  ImGui::PushItemWidth(175);
  if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer))) {
    state->current_creature.set_name(std::string(nameBuffer));
  }
  ImGui::PopItemWidth();

  if (state->is_downloading_image) {
    ImGui::SameLine(200);
    ImGui::Text("Downloading image...");
    imageCallbacks(s);
  }

  if (state->current_creature_texture) {
    ImGui::SameLine(ImGui::GetWindowWidth() - 152);
    ImGui::Image((ImTextureID)(intptr_t)state->current_creature_texture,
                 ImVec2(128, 128));
  } else {
    ImGui::Text("No Image");
  }

  char listNameBuffer[256];
  strncpy(listNameBuffer, state->current_creature.original_list.c_str(),
          sizeof(listNameBuffer));
  listNameBuffer[sizeof(listNameBuffer) - 1] = '\0';

  ImGui::InputText("List Name", listNameBuffer, sizeof(listNameBuffer));

  state->current_creature.original_list = std::string(listNameBuffer);
  ImGui::SameLine();
  if (ImGui::Button("Save to list")) {
    std::vector<dnd::Creature> creatures = {state->current_creature};
    if (std::filesystem::exists(
            "creatures/" + state->current_creature.original_list + ".json")) {
      creatures = parse_creatures_from_file(
          "creatures/" + state->current_creature.original_list + ".json");

      bool found = false;
      for (size_t i = 0; i < creatures.size(); i++) {
        if (creatures[i].get_name() == state->current_creature.get_name()) {
          creatures[i] = state->current_creature;
          found = true;
          break;
        }
      }
      if (!found) {
        creatures.push_back(state->current_creature);
      }
    }
    // Save back to file

    save_creatures_to_file(
        creatures,
        "creatures/" + state->current_creature.original_list + ".json");
  }

  ImGui::Separator();

  Size size = state->current_creature.get_size().value_or(Size::MEDIUM);
  const char* sizeItems[] = {"Gargantuan", "Huge", "Large", "Medium", "Small", "Tiny"};
  int sizeIndex = static_cast<int>(size);
  if (ImGui::Combo("Size", &sizeIndex, sizeItems, IM_ARRAYSIZE(sizeItems))) {
    state->current_creature.set_size(static_cast<Size>(sizeIndex));
  }

  CreatureType type = state->current_creature.get_creature_type().value_or(
      CreatureType::ABERRATION);
  const char* typeItems[] = {"Abberation", "Beast", "Celestial", "Construct",
                             "Dragon",    "Elemental", "Fey",      "Fiend",
                             "Giant",     "Humanoid",  "Monstrosity", "Ooze",  "Plant",     "Undead"};

  int typeIndex = static_cast<int>(type);
  if (ImGui::Combo("Type", &typeIndex, typeItems, IM_ARRAYSIZE(typeItems))) {
    state->current_creature.set_creature_type(static_cast<CreatureType>(typeIndex));
  }

  Alignment alignment =
      state->current_creature.get_alignment().value_or(Alignment::NEUTRAL);

  const char* alignmentItems[] = {
      "Any",          "Any Evil Alignment", "Chaotic Evil",
      "Chaotic Good", "Chaotic Neutral",    "Lawful Evil",
      "Lawful Good",  "Lawful Neutral",     "True Neutral",
      "Neutral",
      "Neutral Evil", "Neutral Good",       "Unaligned"};

  int alignmentIndex = static_cast<int>(alignment);
  if (ImGui::Combo("Alignment", &alignmentIndex, alignmentItems,
                   IM_ARRAYSIZE(alignmentItems))) {
    state->current_creature.set_alignment(static_cast<Alignment>(alignmentIndex));
  }

  ImGui::Separator();

  // Languages
  std::vector<std::string> languages =
      state->current_creature.get_languages().value_or(
          std::vector<std::string>{});

  std::string l = "";

  for (std::string lang : languages)
  {
    l += (lang + ", ");
  }

  char notesBuffer[1024];
  getBufferFromOptionalString(state->current_creature.get_notes(), notesBuffer,
                              sizeof(notesBuffer));
  ImGui::InputTextMultiline("Notes", notesBuffer, sizeof(notesBuffer),
                            ImVec2(0, 0), ImGuiInputTextFlags_WordWrap);
  state->current_creature.set_notes(std::string(notesBuffer));

  char langBuffer[512];
  strncpy(langBuffer, l.c_str(), sizeof(langBuffer));
  langBuffer[sizeof(langBuffer) - 1] = '\0';
  ImGui::InputTextMultiline("Languages", langBuffer, sizeof(langBuffer),
                            ImVec2(0, 0), ImGuiInputTextFlags_WordWrap);

  l = std::string(langBuffer);

  // Split by commas
  std::vector<std::string> langList = splitByDelimiter(l, ',');
  state->current_creature.set_languages(langList);

  ImGui::Separator();

  int armor_class = state->current_creature.get_ac().value_or(10);
  ImGui::InputInt("Armor Class", &armor_class);
  state->current_creature.set_ac(armor_class);

  char hitDiceBuffer[128];
  getBufferFromOptionalString(state->current_creature.get_hit_dice(),
                              hitDiceBuffer, sizeof(hitDiceBuffer));
  ImGui::InputText("Hit Dice", hitDiceBuffer, sizeof(hitDiceBuffer));
  state->current_creature.set_hit_dice(std::string(hitDiceBuffer));

  int hitPoints = state->current_creature.get_max_hit_points();
  ImGui::Text("Max HP: %d", hitPoints);

  // Speed

  Speed sp = state->current_creature.get_speed().value_or(Speed());
  int walk = sp.get_walk().value_or(0);
  int fly = sp.get_fly().value_or(0);
  int swim = sp.get_swim().value_or(0);
  int burrow = sp.get_burrow().value_or(0);
  int climb = sp.get_climb().value_or(0);
  bool hover = sp.get_hover().value_or(false);

  ImGui::Separator();

  ImGui::InputInt("Walk Speed", &walk);
  ImGui::InputInt("Fly Speed", &fly);
  ImGui::InputInt("Swim Speed", &swim);
  ImGui::InputInt("Burrow Speed", &burrow);
  ImGui::InputInt("Climb Speed", &climb);
  ImGui::Checkbox("Hovering", &hover);

  ImGui::Separator();

  state->current_creature.set_speed(
      Speed(walk, fly, swim, burrow, climb, hover));

  // Stats

  int str = state->current_creature.get_stats().value().get_str().value_or(0);
  int dex = state->current_creature.get_stats().value().get_dex().value_or(0);
  int con = state->current_creature.get_stats().value().get_con().value_or(0);
  int intel =
      state->current_creature.get_stats().value().get_modifiers_int().value_or(0);
  int wis = state->current_creature.get_stats().value().get_wis().value_or(0);
  int cha = state->current_creature.get_stats().value().get_cha().value_or(0);

  displayAbilityScore("STR", str);
  displayAbilityScore("DEX", dex, true);
  displayAbilityScore("CON", con);
  displayAbilityScore("INT", intel, true);
  displayAbilityScore("WIS", wis);
  displayAbilityScore("CHA", cha, true);

  ImGui::Separator();

  state->current_creature.set_stats(Modifiers(str, dex, con, intel, wis, cha));

  int str_save =
      state->current_creature.get_saving_throws().value().get_str().value_or(0);
  int dex_save =
      state->current_creature.get_saving_throws().value().get_dex().value_or(0);
  int con_save =
      state->current_creature.get_saving_throws().value().get_con().value_or(0);
  int int_save = state->current_creature.get_saving_throws()
                     .value()
                     .get_modifiers_int()
                     .value_or(0);
  int wis_save =
      state->current_creature.get_saving_throws().value().get_wis().value_or(0);
  int cha_save =
      state->current_creature.get_saving_throws().value().get_cha().value_or(0);

  displayAbilityScore("STR Save", str_save, false, false);
  displayAbilityScore("DEX Save", dex_save, true, false);
  displayAbilityScore("CON Save", con_save, false, false);
  displayAbilityScore("INT Save", int_save, true, false);
  displayAbilityScore("WIS Save", wis_save, false, false);
  displayAbilityScore("CHA Save", cha_save, true, false);
  state->current_creature.set_saving_throws(
      Modifiers(str_save, dex_save, con_save, int_save, wis_save, cha_save));

  std::map<std::string, int> skills =
      state->current_creature.get_skills().value_or(
          std::map<std::string, int>{});

  ImGui::Separator();
  ImGui::Text("Skills:");

  if (ImGui::Button("Add Skill")) {
    skills["New Skill"] = 1;
  }
  for (auto& [skill, value] : skills) {
    if (value == 0) // remove
    {
      skills.erase(skill);
      break;
    }
    char skillBuffer[128];
    strncpy(skillBuffer, skill.c_str(), sizeof(skillBuffer));
    skillBuffer[sizeof(skillBuffer) - 1] = '\0';
    ImGui::PushItemWidth(100);
    if (ImGui::InputText(("##Skill" + skill).c_str(), skillBuffer,
                         sizeof(skillBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
      // Rename key in map
      int v = skills[skill];
      skills.erase(skill);
      skills[std::string(skillBuffer)] = v;
      break;
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
      // Rename key in map
      int v = skills[skill];
      skills.erase(skill);
      skills[std::string(skillBuffer)] = v;
      break;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(85);
    if (ImGui::InputInt(("##Value" + skill).c_str(), &value)) {
      skills[skill] = value;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button(("Remove##" + skill).c_str())) {
      skills.erase(skill);
      break;
    }
  }
  state->current_creature.set_skills(skills);

  std::vector<std::string> traits =
      state->current_creature.get_traits().value_or(std::vector<std::string>{});

  ImGui::Separator();

  ImGui::Text("Traits:");

  if (ImGui::Button("Add Trait")) {
    traits.push_back("New Trait");
  }

  int t_id = 0;
  for (std::string trait : traits)
  {
    char traitBuffer[512];
    strncpy(traitBuffer, trait.c_str(), sizeof(traitBuffer));
    traitBuffer[sizeof(traitBuffer) - 1] = '\0';
    ImGui::InputTextMultiline(("##Trait" + std::to_string(t_id)).c_str(), traitBuffer,
                              sizeof(traitBuffer), ImVec2(0, 0),
                              ImGuiInputTextFlags_WordWrap);

    traits[t_id] = std::string(traitBuffer);
    t_id++;

    if (ImGui::Button(("Remove##Trait" + std::to_string(t_id)).c_str())) {
      traits.erase(traits.begin() + (t_id - 1));
      break;
    }
  }

  state->current_creature.set_traits(traits);

  ImGui::Separator();

  ImGui::Text("Actions:");

  Actions actions = state->current_creature.get_actions().value_or(Actions());

  std::vector<std::string> action_list = actions.get_list().value_or(std::vector<std::string>{});
  if (ImGui::Button("Add Action")) {
      action_list.push_back("New Action");
  }

  int a_id = 0;
  for (std::string action : action_list)
  {
    char actionBuffer[512];
    strncpy(actionBuffer, action.c_str(), sizeof(actionBuffer));
    actionBuffer[sizeof(actionBuffer) - 1] = '\0';
    ImGui::InputTextMultiline(("##Action" + std::to_string(a_id)).c_str(), actionBuffer,
                              sizeof(actionBuffer), ImVec2(0, 0),
                              ImGuiInputTextFlags_WordWrap);
    action_list[a_id] = std::string(actionBuffer);
    a_id++;
    if (ImGui::Button(("Remove##Action" + std::to_string(a_id)).c_str())) {
      action_list.erase(action_list.begin() + (a_id - 1));
      break;
    }
  }

  actions.set_list(action_list);

  state->current_creature.set_actions(actions);

  ImGui::Separator();

  ImGui::Text("Legendary Actions:");
  std::vector<std::string> leg_actions =
      state->current_creature.get_legendary_actions().value_or(
          std::vector<std::string>{});

  if (ImGui::Button("Add Legendary Action")) {
      leg_actions.push_back("New Legendary Action");
  }

  int la_id = 0;
  for (std::string la : leg_actions)
  {
    char laBuffer[512];
    strncpy(laBuffer, la.c_str(), sizeof(laBuffer));
    laBuffer[sizeof(laBuffer) - 1] = '\0';
    ImGui::InputTextMultiline(("##LegendaryAction" + std::to_string(la_id)).c_str(), laBuffer,
                              sizeof(laBuffer), ImVec2(0, 0),
                              ImGuiInputTextFlags_WordWrap);
    leg_actions[la_id] = std::string(laBuffer);
    la_id++;
    if (ImGui::Button(("Remove##LegendaryAction" + std::to_string(la_id)).c_str())) {
      leg_actions.erase(leg_actions.begin() + (la_id - 1));
      break;
    }
  }

  state->current_creature.set_legendary_actions(leg_actions);

  ImGui::Separator();

  ImGui::Text("Reactions:");

  std::vector<std::string> reactions =
      state->current_creature.get_reactions().value_or(
          std::vector<std::string>{});
  if (ImGui::Button("Add Reaction")) {
      reactions.push_back("New Reaction");
  }

  int r_id = 0;
  for (std::string r : reactions)
  {
    char rBuffer[512];
    strncpy(rBuffer, r.c_str(), sizeof(rBuffer));
    rBuffer[sizeof(rBuffer) - 1] = '\0';
    ImGui::InputTextMultiline(("##Reaction" + std::to_string(r_id)).c_str(), rBuffer,
                              sizeof(rBuffer), ImVec2(0, 0),
                              ImGuiInputTextFlags_WordWrap);
    reactions[r_id] = std::string(rBuffer);
    r_id++;
    if (ImGui::Button(("Remove##Reaction" + std::to_string(r_id)).c_str())) {
      reactions.erase(reactions.begin() + (r_id - 1));
      break;
    }
  }

  state->current_creature.set_reactions(reactions);

  ImGui::Separator();

  Challenge c = state->current_creature.get_challenge().value();
  std::string cr = c.get_rating().value_or("0").c_str();
  char crBuffer[16];
  strncpy(crBuffer, cr.c_str(), sizeof(crBuffer));
  crBuffer[sizeof(crBuffer) - 1] = '\0';
  ImGui::InputText("Challenge Rating", crBuffer, sizeof(crBuffer));
  c.set_rating(std::string(crBuffer));
  int exp = c.get_xp().value_or(0);
  ImGui::InputInt("XP", &exp, 100, 1000);
  c.set_xp(exp);
  state->current_creature.set_challenge(c);

  ImGui::Separator();

  char imageUrlBuffer[512];
  getBufferFromOptionalString(state->current_creature.get_image_url(),
                              imageUrlBuffer, sizeof(imageUrlBuffer));
  if (ImGui::InputText("Image URL/Path", imageUrlBuffer, sizeof(imageUrlBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
    state->clear_image = true;
  }
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    state->clear_image = true;
  }
  state->current_creature.set_image_url(std::string(imageUrlBuffer));

  ImGui::EndChild();
}