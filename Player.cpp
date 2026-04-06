#include "Player.h"

Player::Player(int startX, int startY) {
    x = startX;
    y = startY;
    width = 50;  // Máy bay to 50x50 pixel
    height = 50;
    speed = 5;   // Tốc độ 5 pixel mỗi khung hình
    dx = 0;
    dy = 0;
}

Player::~Player() {}

void Player::handleInput() {
    // Lấy trạng thái của toàn bộ bàn phím
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    dx = 0;
    dy = 0;

    if (currentKeyStates[SDL_SCANCODE_UP]) dy = -speed;
    if (currentKeyStates[SDL_SCANCODE_DOWN]) dy = speed;
    if (currentKeyStates[SDL_SCANCODE_LEFT]) dx = -speed;
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) dx = speed;

if (currentKeyStates[SDL_SCANCODE_SPACE]) {
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastShotTime > fireRate) {
        // Tọa độ X: x + 22 (để đạn ra giữa máy bay)
        // Tọa độ Y: y (mũi máy bay)
        // Vận tốc X: -3 (Bay sang trái)
        // Vận tốc Y: -10 (Bay lên trên)
        Bullet* newBullet = new Bullet(x + 22, y, 0, -10);

        bullets.push_back(newBullet);
        lastShotTime = currentTime;
    }
}
}

void Player::update() {
    x += dx;
    y += dy;

    // CHỐNG TRÀN VIỀN: Không cho máy bay bay ra ngoài cửa sổ 800x600
    if (x < 0) x = 0;
    if (x > 800 - width) x = 800 - width;
    if (y < 0) y = 0;
    if (y > 600 - height) y = 600 - height;

    for (int i = 0; i < bullets.size(); i++) {
    bullets[i]->update();
    // Nếu đạn bay ra khỏi màn hình (active = false) thì xóa nó đi
    if (!bullets[i]->isActive()) {
        delete bullets[i];
        bullets.erase(bullets.begin() + i);
        i--;
    }
}
}

void Player::render(SDL_Renderer* renderer) {
    // Vẽ khối chữ nhật màu XANH LÁ đại diện cho máy bay
    SDL_Rect fillRect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // (R, G, B, Alpha)
    SDL_RenderFillRect(renderer, &fillRect);

    for (int i = 0; i < bullets.size(); i++) {
    bullets[i]->render(renderer);
}
}
