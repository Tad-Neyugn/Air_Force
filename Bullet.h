#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>

class Bullet {
public:
    // Yêu cầu truyền vào vận tốc X và Y khi tạo đạn
    Bullet(int startX, int startY, int spdX, int spdY);
    void update();
    void render(SDL_Renderer* renderer);

    bool isActive() { return active; }
    SDL_Rect getRect() { return {x, y, width, height}; }

    void destroy() { active = false; }

private:
    int x, y;
    int width = 10;
    int height = 20;
    int speedX; // Vận tốc ngang (X)
    int speedY; // Vận tốc dọc (Y)
    bool active;
};

#endif
