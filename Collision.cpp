#include "Collision.h"
#include <SDL2/SDL_mixer.h>
#include <vector>

extern Mix_Chunk* explodeSound;

// Hàm 1: Quét va chạm cơ bản giữa 2 hình chữ nhật
bool Collision::check(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w <= b.x || a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y || a.y >= b.y + b.h) return false;
    return true;
}

// Hàm 2: Check đạn đâm trúng đàn gà (Vẫn dùng Node vì gà chưa đổi sang vector)
bool Collision::checkBulletHitHerd(EnemyNode** head, const SDL_Rect& bulletRect) {
    EnemyNode* current = *head;
    EnemyNode* prev = nullptr;

    while (current != nullptr) {
        SDL_Rect enemyRect = { (int)current->data->x, (int)current->data->y, current->data->width, current->data->height };

        if (Collision::check(bulletRect, enemyRect)) {
            if (prev == nullptr) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }

            delete current->data;
            delete current;
            Mix_PlayChannel(-1, explodeSound, 0);
            return true;
        }

        prev = current;
        current = current->next;
    }
    return false;
}

// Hàm 3: Check người chơi đâm vào đàn gà
bool Collision::checkPlayerHitHerd(EnemyNode* head, const SDL_Rect& playerRect) {
    EnemyNode* current = head;
    while (current != nullptr) {
        SDL_Rect enemyRect = { (int)current->data->x, (int)current->data->y, current->data->width, current->data->height };

        if (Collision::check(playerRect, enemyRect)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Hàm 4: Check va chạm với đạn Boss (Dùng Vector - VIP nè)
bool Collision::checkPlayerWithBullets(const SDL_Rect& playerRect, std::vector<Bullet*>& bullets) {
    for (auto b : bullets) {
        if (b->isActive()) {
            SDL_Rect bRect = b->getRect();
            if (Collision::check(playerRect, bRect)) {
                b->destroy(); // Đạn nổ/biến mất
                return true;  // Trúng đòn!
            }
        }
    }
    return false;
}
