#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "HerdControl.h"
#include "Boss.h"
#include "BossBullet.h"

// --- THÊM MỚI: Các trạng thái của Game ---
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_WARNING,
    STATE_GAMEOVER,
    STATE_WIN
};

extern Mix_Chunk* shootSound;
extern Mix_Chunk* explodeSound;

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
    EnemyNode* chickenHerd;
    Boss* myBoss;
    bossBulletNode* bossBullets;

    SDL_Texture* playerTex;
    SDL_Texture* enemyTex;
    SDL_Texture* bossTex;
    SDL_Texture* bossBulletTex;
    SDL_Texture* bulletTex;
    SDL_Texture* backgroundTex;

    // --- THÊM MỚI: Textures cho Menu và Thông báo ---
    SDL_Texture* playBtnTex;
    SDL_Texture* exitBtnTex;
    SDL_Texture* warningTex;
    SDL_Texture* gameOverTex;
    SDL_Texture* winTex;

    uint32_t gameStartTime;
    bool bossSpawned = false;

    float bgY1, bgY2;
    float scrollSpeed;

    // --- THÊM MỚI: Biến quản lý trạng thái và logic mới ---
    GameState currentState;   // Trạng thái hiện tại
    int playerLives;          // Số mạng (3)
    uint32_t warningStartTime; // Thời gian bắt đầu hiện chữ "Boss xuất hiện"

    SDL_Rect btnPlayRect;     // Khung nút Play
    SDL_Rect btnExitRect;     // Khung nút Exit

    void handleCollisions();
};

#endif
