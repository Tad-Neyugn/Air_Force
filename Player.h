#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include "Bullet.h"

class Player {
public:
    Player(int startX, int startY);
    ~Player();
    float getY() { return y; }
    void reduceHealth() { health--; }
    int getHealth() { return health; }

    void handleInput(); // Bắt phím di chuyển
    void update();      // Cập nhật vị trí
    void render(SDL_Renderer* renderer); // Vẽ máy bay ra màn hình
    std::vector<Bullet*> bullets; // Túi chứa vô số viên đạn
    SDL_Rect getRect() {
    return { (int)x, (int)y, width, height };
}

private:
    int x, y;           // Tọa độ hiện tại
    int width, height;  // Kích thước máy bay
    int speed;          // Tốc độ bay
    int dx, dy;         // Hướng di chuyển (Delta X, Delta Y)
    int health = 10;
    bool active = true;


    Uint32 lastShotTime = 0; // Lưu lại thời điểm bắn viên đạn gần nhất
    Uint32 fireRate = 100;   // Khoảng cách bắt buộc giữa 2 viên đạn (200 mili-giây = 0.2 giây)
};

#endif
