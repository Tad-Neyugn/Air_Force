#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>

struct Enemy {
    float x, y;
    float speed;

    int health;
    bool isAlive;

    int type;
    float originX;
    float amplitude;
    float frequency;

    SDL_Texture* texture;
};

Enemy* createEnemy(float x, float y, int type, SDL_Texture* tex);
void updateEnemy(Enemy* e);
void renderEnemy(SDL_Renderer* renderer, Enemy* e);

#endif
