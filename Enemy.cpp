#include "Enemy.h"
#include <cmath>
#include <iostream>
#include "Player.h"

Enemy* createEnemy(float x, float y, int type, int width, int height, SDL_Texture* tex) {
    Enemy* e = new Enemy();
    e->x = x;
    e->y = y;
    e->width = enemyW;
    e->height = enemyH;
    e->originX = x;
    e->type = type;
    e->texture = tex;
    e->active = true;

    if (type == 0) {
        e->health = 1;
        e->Yspeed = 1.0f;
        e->Xspeed = 1.0f;
    }
    else if (type == 1) {
        e->health = 1;
        e->Yspeed = 0.8f;
        e->amplitude = 50.0f;
        e->frequency = 0.005f;
    }
    else if (type == 2) {
        e->health = 1;
        e->Yspeed = 0.7f;
        e->amplitude = 60.0f;
        e->frequency = 0.003f;
    }
    else if (type == 3) {
        e->health = 1;
        e->Yspeed = 1.0f;
        e->Xspeed = 1.5f;
    }

    return e;
}

void updateEnemy(Enemy* e, Player* p) {
    if (!e->active) return;

    // Di chuyển theo trục Y
    e->y += e->Yspeed;

    // Logic di chuyển theo từng loại (Type)
    if (e->type == 1) {
        e->x = e->originX + e->amplitude * sin(SDL_GetTicks() * e->frequency);
    }
    else if (e->type == 0) {
        e->x += e->Xspeed;
    }
    else if (e->type == 2) {
        float time = SDL_GetTicks() * e->frequency;
        e->x = e->originX + cos(time) * e->amplitude;
        e->y += sin(time) * 0.5f;
    }
    else if (e->type == 3) {
        e->Yspeed += 0.03f;
    }

    // Xử lý va chạm với biên trái/phải màn hình
    if (e->x < 0) {
        e->x = 0;
        e->Xspeed = -e->Xspeed;
    }
    else if (e->x + e->width > screenW) {
        e->x = screenW - e->width;
        e->Xspeed = -e->Xspeed;
    }
}

void renderEnemy(SDL_Renderer* renderer, Enemy* e) {
    if (e == nullptr || !e->active) return;

    SDL_Rect destRect = { (int)e->x, (int)e->y, e->width, e->height };

    if (e->texture != nullptr) {
        SDL_RenderCopy(renderer, e->texture, NULL, &destRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_RenderFillRect(renderer, &destRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}


