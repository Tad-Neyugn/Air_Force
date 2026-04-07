#include "Collision.h"

// Hàm 1: Quét va chạm cơ bản
bool Collision::check(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w <= b.x || a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y || a.y >= b.y + b.h) return false;
    return true;
}

// Hàm 2: Quét va chạm đàn gà và tự động xóa con gà bị bắn trúng
bool Collision::checkBulletHitHerd(EnemyNode** head, const SDL_Rect& bulletRect) {
    EnemyNode* current = *head;
    EnemyNode* prev = nullptr;

    while (current != nullptr) {
        // Hộp va chạm của quái
        SDL_Rect enemyRect = { (int)current->data->x, (int)current->data->y, current->data->width, current->data->height };

        if (Collision::check(bulletRect, enemyRect)) {
            // Cắt mắt xích
            if (prev == nullptr) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }

            delete current->data; // Dọn dẹp RAM con gà
            delete current;       // Dọn dẹp Node

            return true; // Báo cáo trúng mục tiêu
        }

        prev = current;
        current = current->next;
    }
    return false;
}

    bool Collision::checkPlayerHitHerd(EnemyNode* head, const SDL_Rect& playerRect) {
    EnemyNode* current = head; // Không cần xóa Node ở đây nên không dùng **
    while (current != nullptr) {
        SDL_Rect enemyRect = { (int)current->data->x, (int)current->data->y, current->data->width, current->data->height };

        if (Collision::check(playerRect, enemyRect)) {
            return true; // Toang! Đâm trúng gà!
        }
        current = current->next;
    }
    return false;
}
// Hàm 4: Quét đạn Boss có găm vào Máy bay không
bool Collision::checkPlayerHitBossBullets(bossBulletNode* head, const SDL_Rect& playerRect) {
    bossBulletNode* currentBullet = head;

    while (currentBullet != nullptr) {
        // Tạo hộp va chạm cho viên đạn Boss

    SDL_Rect bulletRect = {
        (int)currentBullet->data->x,
        (int)currentBullet->data->y,
        (int)currentBullet->data->width,
        (int)currentBullet->data->height
};

        if (Collision::check(playerRect, bulletRect)) {
            return true; // Báo động! Trúng đạn!
        }
        currentBullet = currentBullet->next;
    }
    return false; // An toàn
}

