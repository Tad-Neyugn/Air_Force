#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include <SDL2/SDL.h>
#include <cstdint>
#include "BasicData.h"
#include "BossBullet.h"

struct Boss {
    float x, y;
    int width;
    int height;

    float health;    // Đổi thành float để tính toán sát thương chính xác
    float maxHealth;
    bool active;

    float Yspeed;
    float Xspeed;

    uint32_t lastShootTime;
    int shootInterval;
    int phase;

    SDL_Texture* texture;
};

// Khai báo các hàm xử lý Boss
Boss* createBoss(SDL_Texture* tex);
void updateBoss(Boss *b, float playerY, bossBulletNode** bulletList, SDL_Texture* bulletTex);
void renderBoss(SDL_Renderer* renderer, Boss* b);
void renderBossHP(SDL_Renderer* renderer, Boss* b);

// CHỈ GIỮ LẠI MỘT HÀM takeDamage NÀY
void takeDamage(Boss* b, float damage, bossBulletNode** bulletList);

#endif
