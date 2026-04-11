#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include "Bullet.h"

class Player {
public:
    Player(int startX, int startY);
    ~Player();

    void handleInput();
    void update();
    void render(SDL_Renderer* renderer);

    // Các hàm bổ trợ để Game.cpp sử dụng
    float getX() { return (float)x; }
    float getY() { return (float)y; }
    void reduceHealth() { health--; }
    int getHealth() { return health; }

    SDL_Rect getRect() {
        return { x, y, width, height };
    }

    std::vector<Bullet*> bullets;

    void setX(float xValue) { x = xValue; }
    void setY(float yValue) { y = yValue; }

private:
    int x, y;
    int width, height;
    int speed;
    int dx, dy;
    int health = 10;

    Uint32 lastShotTime = 0;
    Uint32 fireRate = 150;   // Khoảng cách giữa 2 lần bắn (150ms là vừa đẹp)
};

#endif
