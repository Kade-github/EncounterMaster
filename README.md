![Encounter Master Logo](em_logo.png)

> okay I tried to draw a 'dragon seahorse' but like I am NOT good at drawing.. IM SORRY

# EncounterMaster
A D&D 5e encounter manager with creature editing, and sharing.


[![hosted-ninja-vcpkg_submod-cacheoff](https://github.com/Kade-github/EncounterMaster/actions/workflows/cmake.yml/badge.svg)](https://github.com/Kade-github/EncounterMaster/actions/workflows/cmake.yml)

## Features
- Use existing stat blocks as a base.
- Create, edit, and delete creatures with stats, abilities, and actions.
- Save and load encounters with multiple creatures.

## Gallery
<img width="5120" height="2725" alt="image" src="https://github.com/user-attachments/assets/f741ff6b-2bcd-4c72-9e54-d25c7dd1608b" />

## Download
You can download the latest release of EncounterMaster from the [Releases](https://github.com/Kade-github/EncounterMaster/releases/latest) page.

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
