#include "Player.h"
#include <SDL2/SDL_mixer.h>

extern Mix_Chunk* shootSound;

Player::Player(int startX, int startY) {
    x = startX; y = startY;
    width = 120; height = 120;
    speed = 6; dx = 0; dy = 0;
    isInvincible = false;
    invincibleStartTime = 0;
}

Player::~Player() {
    for (auto b : bullets) delete b;
    bullets.clear();
}

void Player::handleInput() {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    dx = 0; dy = 0;
    if (state[SDL_SCANCODE_UP]) dy = -speed;
    if (state[SDL_SCANCODE_DOWN]) dy = speed;
    if (state[SDL_SCANCODE_LEFT]) dx = -speed;
    if (state[SDL_SCANCODE_RIGHT]) dx = speed;

    if (state[SDL_SCANCODE_SPACE]) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastShotTime > fireRate) {
            bullets.push_back(new Bullet(x + width/2 - 5, y, 0, -8));
            if (shootSound) Mix_PlayChannel(-1, shootSound, 0);
            lastShotTime = currentTime;
        }
    }
}

void Player::update() {
    x += dx; y += dy;
    if (x < 0) x = 0;
    if (x > 800 - width) x = 800 - width;
    if (y < 0) y = 0;
    if (y > 600 - height) y = 600 - height;

    auto it = bullets.begin();
    while (it != bullets.end()) {
        (*it)->update();
        if (!(*it)->isActive()) {
            delete *it;
            it = bullets.erase(it);
        } else ++it;
    }

    if (isInvincible && SDL_GetTicks() - invincibleStartTime > 2000) {
        isInvincible = false;
    }
}

void Player::makeInvincible() {
    isInvincible = true;
    invincibleStartTime = SDL_GetTicks();
}

void Player::render(SDL_Renderer* renderer) {
    // Nếu đang bất tử thì vẽ nhấp nháy
    if (isInvincible && (SDL_GetTicks() / 100) % 2 == 0) return;

    SDL_Rect rect = { x, y, width, height };
    // Phần render texture ông tự thêm vào Game.cpp hoặc dùng hàm vẽ mặc định
}
