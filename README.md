![Encounter Master Logo](em_logo.png)

> okay I tried to draw a 'dragon seahorse' but like I am NOT good at drawing.. IM SORRY

# EncounterMaster
A D&D 5e encounter manager with creature editing, and sharing.


[![hosted-ninja-vcpkg_submod-cacheoff](https://github.com/Kade-github/EncounterMaster/actions/workflows/cmake.yml/badge.svg)](https://github.com/Kade-github/EncounterMaster/actions/workflows/cmake.yml)

## Main Features (Headliners)
- Use existing stat blocks as a base.
- Create, edit, and delete creatures with stats, abilities, and actions.
- Save and load encounters with multiple creatures.
- Cross-platform support (Windows, Linux, MacOS).
- It's free, and open source! You don't have to pay a dime!

## Download
You can download the latest release of EncounterMaster from the [Releases](https://github.com/Kade-github/EncounterMaster/releases/latest) page.

### Everything else :P
- Export and import creatures as JSON files for easy sharing.
- Search and filter creatures by list, name, or regex.
- Add custom notes to creatures in encounters for later.
- Create and save creatures to specific lists.
- Change pretty much every single thing about a creature (besides a few damage things, which might come later... shh), the exhaustive list is:
  - Name
  - Size
  - Type
  - Alignment
  - Armor Class
  - Hit Points (with dice formula support)
  - Speed (multiple types)
  - Ability Scores (STR, DEX, CON, INT, WIS, CHA)
  - Skills (Or traits, if you want to be fancy)
  - Languages
  - Challenge Rating
  - Actions (name and description)
  - Legendary Actions (name and description)
  - Reactions (name and description)
- Automatic display for modifiers.
- Download lists from `creature_mirrors.txt`, which is a simple text file with URLs to JSON creature lists. (You can make your own and add it to the file if you want)
- Everything important is keybinded.
- Automatically downloading and caching images from creatures (supports local files too!)
- It's pretty lightweight and fast.

## Gallery
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/5f6c0f8d-12ad-46d8-ac31-0ae77e46fa4b" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/4906d2cc-02e2-4f87-8764-f6d0d6d26dc9" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/73b7707f-6d6c-4ea4-a06a-2c5b414b1ff0" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/5e0703c8-597d-41cb-b44d-07cca0d7b325" />

## Building from Source
### Prerequisites
- CMake 3.15 or higher
- A C++ compiler that supports C++17
- VCPKG for dependency management

### Steps
1. Clone the repo.
2. Initialize and update submodules:
   ```bash
   git submodule update --init --recursive
   ```
3. Install dependencies using VCPKG:
   ```bash
   vcpkg install
   ```
4. CMake configuration:
   ```bash
   cmake -B build -S .
   ```
5. Build the project:
   ```bash
   cmake --build build
   ```

Or, you can use visual studio or something lol.

After you build, make sure to copy the `assets` folder to the same directory as the executable. (Otherwise it will get mad)

Make sure to also copy any libraries you might need as well (mostly on windows).
