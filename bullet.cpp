#include "Bullet.h"

Bullet::Bullet(int startX, int startY, int spdX, int spdY) {
    x = startX;
    y = startY;
    speedX = spdX;
    speedY = spdY;
    active = true;
}

void Bullet::update() {
    x += speedX; // Đạn bay ngang
    y += speedY; // Đạn bay dọc

    // Nếu đạn bay ra khỏi 4 cạnh màn hình (giả sử màn hình rộng 800, cao 600) thì xóa
    if (y < 0 || y > 600 || x < 0 || x > 800) {
        active = false;
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Tô màu Vàng
    SDL_RenderFillRect(renderer, &rect);
}
