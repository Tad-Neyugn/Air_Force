#include "Boss.h"
#include "BossBullet.h"
#include <cmath>

void bossAttack(Boss* b, bossBulletNode** bulletList, SDL_Texture* bulletTex);

Boss* createBoss(SDL_Texture* tex) {
    Boss* b = new Boss();

    b->width = bossW;
    b->height = bossH;

    b->x = (screenW - b->width)/2;
    b->y = 100;

    b->maxHealth = 500;
    b->health = b->maxHealth;
    b->active = true;

    b->Xspeed = 0.0f;
    b->Yspeed = 0.15f;

    b->phase = 1;
    b->shootInterval = 1000;
    b->lastShootTime = SDL_GetTicks();

    b->texture = tex;

    return b;
}

void updateBoss(Boss *b, float playerY, bossBulletNode** bulletList, SDL_Texture* bulletTex) {
    if(b == nullptr || !b->active) return;

    b->y += b->Yspeed;
    b->x = ((screenW - b->width) / 2) + sin(SDL_GetTicks() * 0.001f) * 50;

    if(b->phase == 1 && b->health <= b->maxHealth*0.3f) {
        b->phase = 2;
        b->shootInterval = 750;
        b->Yspeed =0.25f;
    }

    uint32_t currentTime = SDL_GetTicks();
    if(currentTime > b->lastShootTime + b->shootInterval) {
        bossAttack(b, bulletList, bulletTex);
        b->lastShootTime = currentTime;
    }

    //if (b->y + b->height >= playerY)
}

void bossAttack(Boss* b, bossBulletNode** bulletList, SDL_Texture* bulletTex) {
    if (b == nullptr || !b->active) return;

    int numBullets = (b->phase == 1) ? 2 : 3;

    for (int i = 0; i < numBullets; i++) {
        float spawnX = b->x + 10 + (rand() % (b->width - 20));
        float spawnY = b->y + b->height;
        addBossBullet(bulletList, spawnX, spawnY, 0.0f, 3.0f, 10, 20, bulletTex);
    }
}

void renderBoss(SDL_Renderer* renderer, Boss* b) {
    if (b == nullptr || !b->active) return;

    SDL_Rect destRect = { (int)b->x, (int)b->y, b->width, b->height };

    if (b->texture != nullptr) {
        SDL_RenderCopy(renderer, b->texture, NULL, &destRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &destRect);
    }

    renderBossHP(renderer, b);
}

void renderBossHP(SDL_Renderer* renderer, Boss* b) {
    if (b == nullptr || !b->active) return;

    float hpPercent = (float)b->health / b->maxHealth;
    if (hpPercent < 0) hpPercent = 0;

    SDL_Rect borderRect = { (screenW - 600) / 2, 20, 600, 25 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &borderRect);

    SDL_Rect healthRect = { borderRect.x + 2, borderRect.y + 2,
                            (int)((borderRect.w - 4) * hpPercent), borderRect.h - 4 };

    if (b->phase == 2) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_RenderFillRect(renderer, &healthRect);
}

void takeDamage(Boss* b, float damage, bossBulletNode** bulletList) {
    if(b == nullptr || !b->active) return;

    b->health -= damage;
    if(b->health <= 0) {
        b->health = 0;
        b->active = false;
        //Co the them effect gi do
    }

    if(bulletList != nullptr && *bulletList != nullptr) clearBossBullets(bulletList);
}

