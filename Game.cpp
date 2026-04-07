#include "Game.h"
#include "BasicData.h"
#include "HerdControl.h"
#include "Collision.h"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>

Game::Game() {
    chickenHerd = nullptr;
}
Game::~Game() {}

SDL_Texture* loadTexture(const char* fileName, SDL_Renderer* ren) {
    SDL_Surface* tempSurface = SDL_LoadBMP(fileName);

    if (tempSurface == NULL) {
        std::cout << "Khong load duoc anh (kiem tra file .bmp): " << fileName << std::endl;
        return NULL;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "SDL khoi tao thanh cong!" << std::endl;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, false);
        if (window) std::cout << "Tao cua so thanh cong!" << std::endl;

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            std::cout << "Tao renderer thanh cong!" << std::endl;
        }
        isRunning = true;

        gameStartTime = SDL_GetTicks();
        bossSpawned = false;
        myBoss = nullptr;
        bossBullets = nullptr;

        bossTex = nullptr;
        bossBulletTex = nullptr;

        player = new Player(375, 500);

        chickenHerd = nullptr;
        srand(time(NULL));
    } else {
        isRunning = false;
    }

    backgroundTex1 = loadTexture("asset/bg1.bmp", renderer);
    backgroundTex2 = loadTexture("asset/bg2.bmp", renderer);
    bgY1 = 0;
    bgY2 = -600;
    scrollSpeed = 0.5f;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_h) {
                    if (bossSpawned && myBoss != nullptr && myBoss->active) {
                        takeDamage(myBoss, 50, &bossBullets);
                        std::cout << "DEBUG: Boss hit! Current HP: " << myBoss->health << std::endl;
                        if (myBoss->phase == 2) {
                            std::cout << "WARNING: Boss entered PHASE 2!" << std::endl;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    if (player) {
        player->handleInput();
    }
}

void Game::update() {
    if (player == nullptr) return;

    player->update();

    uint32_t currentTime = SDL_GetTicks();
    uint32_t elapsedTime = currentTime - gameStartTime;

    // --- NHÁNH 1: TRƯỚC 2 PHÚT (Giai đoạn Gà) ---
    if (elapsedTime < 12000) {
        static int Timer = 0;
        Timer++;
        if(Timer >= 120) {
            float randomX = (rand() % 700) + 50;
            int type = rand() % 4;
            addEnemy(&chickenHerd, randomX, -50, type, nullptr);
            Timer = 0;
        }
        updateHerd(&chickenHerd, player);
    }
    // --- NHÁNH 2: ĐẾN GIỜ TRÙM CUỐI XUẤT HIỆN ---
    else if (!bossSpawned) {
        clearHerd(&chickenHerd);
        myBoss = createBoss(bossTex);
        bossSpawned = true;
        std::cout << "WARNING: UNKNOWN AEROSPACE OBJECT DETECTED!" << std::endl;
    }
    // --- NHÁNH 3: CHIẾN ĐẤU VỚI BOSS ---
    else {
        if (myBoss && myBoss->active) {
            updateBoss(myBoss, player->getY(), &bossBullets, bossBulletTex);
            updateBossBullet(&bossBullets);
        }
    }
    handleCollisions();

    // Cập nhật background
    bgY1 += scrollSpeed;
    bgY2 += scrollSpeed;

    if (bgY1 >= 600) {
        bgY1 = bgY2 - 600;
    }
    if (bgY2 >= 600) {
        bgY2 = bgY1 - 600;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    SDL_Rect rect1 = { 0, (int)bgY1, 800, 600 };
    SDL_Rect rect2 = { 0, (int)bgY2, 800, 600 };

    SDL_RenderCopy(renderer, backgroundTex1, NULL, &rect1);
    SDL_RenderCopy(renderer, backgroundTex2, NULL, &rect2);

    if (player) player->render(renderer);
    if(chickenHerd != nullptr) renderHerd(chickenHerd, renderer);

    if (bossSpawned && myBoss != nullptr) {
        renderBoss(renderer, myBoss);
        renderBossBullet(renderer, bossBullets);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    clearHerd(&chickenHerd); // Xóa sạch đàn gà để giải phóng RAM

    if (player) {
        delete player;
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
void Game::handleCollisions() {
    if (player == nullptr) return;
    // ==========================================
    // XÉT VA CHẠM ĐẠN CỦA PLAYER VỚI ĐÀN GÀ VÀ BOSS
    // ==========================================
    if (player) {
        for (int i = 0; i < player->bullets.size(); i++) {
            Bullet* b = player->bullets[i];

            if (b->isActive()) {
                // 1. Nhánh bắn Boss
                if (bossSpawned && myBoss != nullptr && myBoss->active) {
                    SDL_Rect bossRect = { (int)myBoss->x, (int)myBoss->y, myBoss->width, myBoss->height };
                    if (Collision::check(b->getRect(), bossRect)) {
                        b->destroy();
                        takeDamage(myBoss, 10, &bossBullets);

                        std::cout << "Player ban trung Boss! Mau Boss: " << myBoss->health << std::endl;
                        continue;
                    }
                }

                // 2. Nhánh bắn Gà
                if (!bossSpawned && chickenHerd != nullptr) {
                    // Gọi hàm quét va chạm từ file Collision (Đã cập nhật)
                    if (Collision::checkBulletHitHerd(&chickenHerd, b->getRect())) {
                        b->destroy(); // Viên đạn nổ tung
                        std::cout << "BUM! 1 con ga da bi ban rung!" << std::endl;
                        continue;
                    }
                }
            }
        }
    }
    // ==========================================
    // THÊM MỚI: XÉT VA CHẠM MÁY BAY VỚI KẺ ĐỊCH (THUA GAME)
    // ==========================================
    if (player) {
        SDL_Rect playerRect = player->getRect();

        // 1. Máy bay đâm vào Gà
        if (!bossSpawned && chickenHerd != nullptr) {
            if (Collision::checkPlayerHitHerd(chickenHerd, playerRect)) {
                std::cout << "MAY BAY DAM VAO GA! GAME OVER!" << std::endl;
                isRunning = false; // Tạm thời cho văng game để biết là đã chết
            }
        }

        // 2. Máy bay đâm thẳng vào thân Boss
        if (bossSpawned && myBoss != nullptr && myBoss->active) {
            SDL_Rect bossRect = { (int)myBoss->x, (int)myBoss->y, myBoss->width, myBoss->height };
            if (Collision::check(playerRect, bossRect)) {
                std::cout << "MAY BAY DAM VAO BOSS! GAME OVER!" << std::endl;
                isRunning = false; // Tạm thời cho văng game
            }
        }
    }

    SDL_Rect playerRect = player->getRect();

    if (bossSpawned && bossBullets != nullptr) {
        // Gọi cái Radar (Hàm 4) mà bro vừa viết xong
        if (Collision::checkPlayerHitBossBullets(bossBullets, playerRect)) {
            std::cout << "TOANG! MAY BAY TRUNG DAN BOSS! GAME OVER!" << std::endl;
            isRunning = false; // Kết thúc game

        }
    }
}
