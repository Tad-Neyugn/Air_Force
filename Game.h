#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>     // Lưu ý: Để <SDL.h> cho khỏi bị lỗi đường dẫn như lúc đầu nhé
#include <iostream>
#include "Player.h"
#include "Enemy.h"   // <--- THÊM DÒNG NÀY: Khai báo thư viện Quái

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;

    Player* player;
    Enemy* testEnemy; // <--- THÊM DÒNG NÀY: Tạo ra một con quái để test
};

#endif
