#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "BasicData.h"
#include "Player.h"
#include <SDL2/SDL.h>

struct Enemy {
    float x, y;
    float Xspeed;
    float Yspeed;
    int health;
    bool active;
    int width = enemyW;
    int height = enemyH;

    int type;
    float originX;
    float amplitude;
    float frequency;

    SDL_Texture* texture;
};

Enemy* createEnemy(float x, float y, int type, int width, int height, SDL_Texture* tex);
void updateEnemy(Enemy* e, Player* p);
void renderEnemy(SDL_Renderer* renderer, Enemy* e);

#endif

