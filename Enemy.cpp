#include "Enemy.h"
#include <cmath>

Enemy* createEnemy(float x, float y, int type, SDL_Texture* tex) {
    Enemy* e = new Enemy();
    e->x = x;
    e->y = y;
    e->originX = x;
    e->type = type;
    e->texture = tex;
    e->isAlive = true;

    if (type == 0) {
        e->health = 1;
        e->speed = 1.0f;
    } else if (type == 1) {
        e->health = 2;
        e->speed = 0.8f;
        e->amplitude = 50.0f;
        e->frequency = 0.005f;
    }

    return e;
}

void updateEnemy(Enemy* e) {
    if (!e->isAlive) return;

    e->y += e->speed;

    if (e->type == 1) {
        e->x = e->originX + e->amplitude * sin(SDL_GetTicks() * e->frequency);
    }
}

void renderEnemy(SDL_Renderer* renderer, Enemy* e) {
    if (!e->isAlive) return;

    SDL_Rect destRect = { (int)e->x, (int)e->y, 60, 60 };
    SDL_RenderCopy(renderer, e->texture, NULL, &destRect);
}
