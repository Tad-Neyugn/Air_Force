#ifndef BOSSBULLET_H_INCLUDED
#define BOSSBULLET_H_INCLUDED
#include <SDL2/SDL.h>
#include "BasicData.h"

struct bossBullet {
    int x, y;
    int width, height;
    float speedX, speedY;
    bool active;
    SDL_Texture* texture;
};

struct bossBulletNode {
    bossBullet* data;
    bossBulletNode* next;
};

void addBossBullet(bossBulletNode** head, float x, float y, float sX, float sY, int w, int h, SDL_Texture* tex);
void updateBossBullet(bossBulletNode** head);
void renderBossBullet(SDL_Renderer* renderer, bossBulletNode* head);
void clearBossBullets(bossBulletNode** head);
void renderBossBullet(SDL_Renderer* renderer, bossBulletNode* head, SDL_Texture* tex);
#endif // BOSSBULLET_H_INCLUDED
