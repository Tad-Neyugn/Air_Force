#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>        // Thêm thư viện vector
#include "BasicData.h"
#include "Bullet.h"      // Dùng Bullet.h thay vì BossBullet.h

struct Boss {
    float x, y;
    int width;
    int height;

    float health;
    float maxHealth;
    bool active;

    float Yspeed;
    float Xspeed;

    uint32_t lastShootTime;
    int shootInterval;
    int phase;

    SDL_Texture* texture;
};

// Khai báo lại các hàm sử dụng std::vector thay vì bossBulletNode
Boss* createBoss(SDL_Texture* tex);
void updateBoss(Boss *b, float playerY, std::vector<Bullet*>& bulletList); // Chỗ này quan trọng nè
void renderBoss(SDL_Renderer* renderer, Boss* b);
void renderBossHP(SDL_Renderer* renderer, Boss* b);
void takeDamage(Boss* b, float damage); // Bỏ cái bulletList thừa đi

#endif
