#ifndef _CREATURE_EDIT_H
#define _CREATURE_EDIT_H

#include <SDL3/SDL.h>

SDL_Texture* loadTextureFromFile(SDL_Renderer* renderer,
                                 const std::string& filePath);

void saveCreature(void* state);

void displayCreatureEdit(void* state);

#endif