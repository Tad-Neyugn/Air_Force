#include "Boss.h"
#include <cmath>

void bossAttack(Boss* b, bossBulletNode** bulletList, SDL_Texture* bulletTex);

Boss* createBoss(SDL_Texture* tex) {
    Boss* b = new Boss();
    b->width = 300;  // Kích thước nên để tầm này cho cân đối 800x600
    b->height = 250;
    b->x = (screenW - b->width) / 2;
    b->y = -200;      // Xuất hiện từ từ phía trên màn hình

    b->maxHealth = 500;
    b->health = b->maxHealth;
    b->active = true;

    b->Xspeed = 0.0f;
    b->Yspeed = 0.5f; // Tốc độ bay xuống ban đầu

    b->phase = 1;
    b->shootInterval = 1000;
    b->lastShootTime = SDL_GetTicks();
    b->texture = tex;

    return b;
}

void updateBoss(Boss *b, float playerY, bossBulletNode** bulletList, SDL_Texture* bulletTex) {
    if (b == nullptr || !b->active) return;

    // Boss bay xuống đến vị trí y=50 thì dừng lại và lượn ngang
    if (b->y < 50) {
        b->y += b->Yspeed;
    } else {
        // Lượn ngang hình Sin cho khó bắn
        b->x = ((screenW - b->width) / 2) + sin(SDL_GetTicks() * 0.001f) * 150;
    }

    // Chuyển Phase khi máu yếu
    if (b->phase == 1 && b->health <= b->maxHealth * 0.3f) {
        b->phase = 2;
        b->shootInterval = 600; // Bắn nhanh hơn
    }

    uint32_t currentTime = SDL_GetTicks();
    if (currentTime > b->lastShootTime + b->shootInterval) {
        bossAttack(b, bulletList, bulletTex);
        b->lastShootTime = currentTime;
    }
}

void bossAttack(Boss* b, bossBulletNode** bulletList, SDL_Texture* bulletTex) {
    if (b == nullptr || !b->active) return;
    int numBullets = (b->phase == 1) ? 2 : 4;
    for (int i = 0; i < numBullets; i++) {
        float spawnX = b->x + (b->width / (numBullets + 1)) * (i + 1);
        addBossBullet(bulletList, spawnX, b->y + b->height, 0.0f, 4.0f, 20, 20, bulletTex);
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
    }
    renderBossHP(renderer, b);
}

void renderBossHP(SDL_Renderer* renderer, Boss* b) {
    float hpPercent = b->health / b->maxHealth;
    SDL_Rect borderRect = { 100, 20, 600, 20 };
    SDL_Rect healthRect = { 102, 22, (int)(596 * hpPercent), 16 };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Khung trắng
    SDL_RenderDrawRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);     // Máu đỏ
    SDL_RenderFillRect(renderer, &healthRect);
}

void takeDamage(Boss* b, float damage, bossBulletNode** bulletList) {
    if (b == nullptr || !b->active) return;
    b->health -= damage;
    if (b->health <= 0) {
        b->active = false;
    }
}

