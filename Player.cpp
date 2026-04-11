#include "Player.h"
#include <SDL2/SDL_mixer.h>

// MƯỢN biến shootSound từ Game.cpp
extern Mix_Chunk* shootSound;

Player::Player(int startX, int startY) {
    x = startX;
    y = startY;
    width = 80;
    height = 80;
    speed = 5;
    dx = 0;
    dy = 0;
}

Player::~Player() {
    // Dọn dẹp đạn khi xóa player
    for (auto b : bullets) {
        delete b;
    }
    bullets.clear();
}

void Player::handleInput() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    dx = 0;
    dy = 0;

    // Di chuyển
    if (currentKeyStates[SDL_SCANCODE_UP]) dy = -speed;
    if (currentKeyStates[SDL_SCANCODE_DOWN]) dy = speed;
    if (currentKeyStates[SDL_SCANCODE_LEFT]) dx = -speed;
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) dx = speed;

    // Bắn đạn
    if (currentKeyStates[SDL_SCANCODE_SPACE]) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastShotTime > fireRate) {
            // Tạo đạn ở giữa mũi máy bay
            Bullet* newBullet = new Bullet(x + (width/2) - 2, y, 0, -10);
            bullets.push_back(newBullet);

            // PHÁT ÂM THANH: Kiểm tra kỹ tránh treo máy
            if (shootSound != nullptr) {
                Mix_PlayChannel(-1, shootSound, 0);
            }

            lastShotTime = currentTime;
        }
    }
}

void Player::update() {
    x += dx;
    y += dy;

    // Giới hạn máy bay trong màn hình 800x600
    if (x < 0) x = 0;
    if (x > 800 - width) x = 800 - width;
    if (y < 0) y = 0;
    if (y > 600 - height) y = 600 - height;

    // Cập nhật đạn
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i]->update();
        if (!bullets[i]->isActive()) {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }
}

void Player::render(SDL_Renderer* renderer) {
    // Lưu ý: Trong Game::render chúng ta đã vẽ ảnh PNG rồi
    // Hàm này chỉ để vẽ khối màu dự phòng nếu ảnh bị lỗi
    SDL_Rect fillRect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &fillRect);
}
