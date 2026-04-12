#ifndef HERDCONTROL_H_INCLUDED
#define HERDCONTROL_H_INCLUDED
#include "Enemy.h"
#include <SDL2/SDL.h>
#include "BasicData.h"

struct EnemyNode {
    Enemy* data;
    EnemyNode* next;
};

void addEnemy(EnemyNode** head, float x, float y, int type, SDL_Texture* tex);
void updateHerd(EnemyNode** head, Player* p);
void clearHerd(EnemyNode** head);

void renderHerd(EnemyNode* head, SDL_Renderer* renderer, SDL_Texture* tex);
#endif
