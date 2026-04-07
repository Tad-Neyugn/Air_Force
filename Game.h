#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>     // Lưu ý: Để <SDL.h> cho khỏi bị lỗi đường dẫn như lúc đầu nhé
#include <iostream>
#include <cstdint>
#include "Player.h"
#include "Enemy.h"   // <--- THÊM DÒNG NÀY: Khai báo thư viện Quái
#include "HerdControl.h"
#include "Boss.h"
#include "BossBullet.h"

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
    Enemy* testEnemy;
    EnemyNode* chickenHerd;

    uint32_t gameStartTime;
    bool bossSpawned;
    Boss* myBoss;
    bossBulletNode* bossBullets;

    SDL_Texture* bossTex;
    SDL_Texture* bossBulletTex;

    const uint32_t BOSS_TRIGGER_TIME = 120000;

    float bgY1;
    float bgY2;
    float scrollSpeed;

    SDL_Texture* backgroundTex1;
    SDL_Texture* backgroundTex2;
};

#endif
