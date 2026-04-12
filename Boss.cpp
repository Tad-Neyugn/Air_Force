#include "Boss.h"
#include "BasicData.h"
#include <cmath>
#include <vector>
#include "Bullet.h" // Để dùng class Bullet thay cho bossBulletNode

// Khai báo hàm nội bộ (Helper function)
void bossAttack(Boss* b, std::vector<Bullet*>& bulletList);

Boss* createBoss(SDL_Texture* tex) {
    Boss* b = new Boss();

    b->width = bossW;
    b->height = bossH;
    b->x = (screenW - b->width) / 2;
    b->y = -200;      // Xuất hiện từ từ từ phía trên

    b->maxHealth = bossMaxHP;
    b->health = b->maxHealth;
    b->active = true;

    b->Xspeed = 0.0f;
    b->Yspeed = bossBaseSpeed;

    b->phase = 1;
    b->shootInterval = 1000;
    b->lastShootTime = SDL_GetTicks();
    b->texture = tex;

    return b;
}

// Sửa tham số truyền vào thành std::vector<Bullet*>&
void updateBoss(Boss *b, float playerY, std::vector<Bullet*>& bulletList) {
    if (b == nullptr || !b->active) return;

    // Boss bay xuống đến vị trí y=50 thì dừng lại và lượn ngang
    if (b->y < 50) {
        b->y += b->Yspeed;
    } else {
        // Lượn ngang hình Sin
        b->x = ((screenW - b->width) / 2) + sin(SDL_GetTicks() * 0.001f) * 150;
    }

    // Chuyển Phase khi máu yếu
    if (b->phase == 1 && b->health <= b->maxHealth * 0.3f) {
        b->phase = 2;
        b->shootInterval = 600; // Bắn nhanh hơn
    }

    uint32_t currentTime = SDL_GetTicks();
    if (currentTime > b->lastShootTime + b->shootInterval) {
        bossAttack(b, bulletList); // Truyền vector vào đây
        b->lastShootTime = currentTime;
    }
}

// Hàm bắn đạn sử dụng Vector
void bossAttack(Boss* b, std::vector<Bullet*>& bulletList) {
    if (b == nullptr || !b->active) return;
    int numBullets = (b->phase == 1) ? 2 : 4;
    for (int i = 0; i < numBullets; i++) {
        float spawnX = b->x + (b->width / (numBullets + 1)) * (i + 1);
        // Tạo đối tượng Bullet mới và đẩy vào vector
        // Tham số: x, y, speedX, speedY
        bulletList.push_back(new Bullet(spawnX, b->y + b->height, 0, 5));
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
    if (hpPercent < 0) hpPercent = 0;

    SDL_Rect borderRect = { 100, 20, 600, 20 };
    SDL_Rect healthRect = { 102, 22, (int)(596 * hpPercent), 16 };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Khung trắng
    SDL_RenderDrawRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);     // Máu đỏ
    SDL_RenderFillRect(renderer, &healthRect);
}

void takeDamage(Boss* b, float damage) {
    if (b == nullptr || !b->active) return;
    b->health -= damage;
    if (b->health <= 0) {
        b->health = 0;
        b->active = false;
    }
}
