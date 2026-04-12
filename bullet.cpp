#include "Bullet.h"
#include "BasicData.h"

Bullet::Bullet(int startX, int startY, int spdX, int spdY) {
    x = startX; y = startY;
    speedX = spdX; speedY = spdY;
    active = true;
    width = 15;  height = 30; // Chỉnh lại cho cân đối
}

void Bullet::update() {
    x += speedX;
    y += speedY;
    // Dùng hằng số thay vì 600, 800
    if (y < -50 || y > screenH + 50 || x < -50 || x > screenW + 50) {
        active = false;
    }
}


void Bullet::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Tô màu Vàng
    SDL_RenderFillRect(renderer, &rect);
}
