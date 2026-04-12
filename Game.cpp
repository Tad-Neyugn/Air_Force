#include "Game.h"
#include "Collision.h"
#include "BasicData.h"
#include <ctime>
#include <algorithm>

Mix_Chunk* shootSound = nullptr;
Mix_Chunk* explodeSound = nullptr;

Game::Game() {
    isRunning = false;
    chickenHerd = nullptr;
    myBoss = nullptr;
    currentState = STATE_MENU;
    playerLives = 3;
    btnPlayRect = { 300, 250, 200, 80 };
    btnExitRect = { 300, 350, 200, 80 };
    bossSpawned = false;
    stateStartTime = 0;
}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        IMG_Init(IMG_INIT_PNG);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        isRunning = true;

        playerTex = IMG_LoadTexture(renderer, "assets/player.png");
        enemyTex = IMG_LoadTexture(renderer, "assets/chicken.png");
        bulletTex = IMG_LoadTexture(renderer, "assets/bullet.png");
        backgroundTex = IMG_LoadTexture(renderer, "assets/bg1.png");
        backgroundTex2 = IMG_LoadTexture(renderer, "assets/bg2.png");
        bossTex = IMG_LoadTexture(renderer, "assets/boss.png");
        bossBulletTex = IMG_LoadTexture(renderer, "assets/boss_bullet.png");
        playBtnTex = IMG_LoadTexture(renderer, "assets/play.png");
        exitBtnTex = IMG_LoadTexture(renderer, "assets/exit.png");
        warningTex = IMG_LoadTexture(renderer, "assets/warning.png");
        gameOverTex = IMG_LoadTexture(renderer, "assets/gameover.png");
        winTex = IMG_LoadTexture(renderer, "assets/win.png");

        shootSound = Mix_LoadWAV("assets/shoot.wav");
        explodeSound = Mix_LoadWAV("assets/explode.wav");
        player = new Player(375, 500);
        gameStartTime = SDL_GetTicks();
        bgY1 = 0; bgY2 = -screenH;
        scrollSpeed = 0.8f;
        srand(time(NULL));
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            SDL_Point mousePos = { mx, my }; // Fix memory leak ở đây

            if (currentState == STATE_MENU) {
                if (SDL_PointInRect(&mousePos, &btnPlayRect)) {
                    playerLives = 3;
                    bossSpawned = false;
                    if (myBoss) { delete myBoss; myBoss = nullptr; }
                    clearHerd(&chickenHerd);

                    for (auto b : bossBullets) delete b;
                    bossBullets.clear();

                    currentState = STATE_PLAYING;
                    gameStartTime = SDL_GetTicks();
                    player->isInvincible = false;
                    player->setX(375); player->setY(500);
                    player->bullets.clear();
                }
                if (SDL_PointInRect(&mousePos, &btnExitRect)) isRunning = false;
            }
        }
    }
    if (player && (currentState == STATE_PLAYING || currentState == STATE_WARNING))
        player->handleInput();
}

void Game::update() {
    bgY1 += scrollSpeed; bgY2 += scrollSpeed;
    if (bgY1 >= screenH) bgY1 = bgY2 - screenH;
    if (bgY2 >= screenH) bgY2 = bgY1 - screenH;

    uint32_t elapsedTime = SDL_GetTicks() - gameStartTime;

    switch (currentState) {
        case STATE_PLAYING: {
            player->update();

            if (!bossSpawned) {
                // 1. SINH GÀ: Chỉ sinh khi chưa tới 20 giây
                if (elapsedTime < 20000) {
                    static int spawnTimer = 0;
                    if (++spawnTimer >= 60) {
                        float randomX = (rand() % (screenW - 100)) + 50;
                        addEnemy(&chickenHerd, randomX, -50, rand() % 4, enemyTex);
                        spawnTimer = 0;
                    }
                }

                // 2. CẬP NHẬT GÀ: Luôn cập nhật để gà di chuyển và bị bắn
                updateHerd(&chickenHerd, player);

                // 3. CHECK GÀ XỔNG: Vẫn giữ logic trừ mạng khi gà bay mất
                EnemyNode* temp = chickenHerd;
                while (temp != nullptr) {
                    if (temp->data->active && temp->data->y > screenH) {
                        temp->data->active = false;
                        if (!player->isInvincible) {
                            playerLives--;
                            Mix_PlayChannel(-1, explodeSound, 0);
                            if (playerLives <= 0) {
                                currentState = STATE_GAMEOVER;
                                stateStartTime = SDL_GetTicks();
                                return;
                            } else {
                                player->makeInvincible();
                            }
                        }
                    }
                    temp = temp->next;
                }

                // 4. ĐIỀU KIỆN RA BOSS (ĐÚNG Ý BRO):
                // Phải THỎA MÃN CẢ HAI: Hết 20 giây VÀ Không còn con gà nào trên màn hình
                if (elapsedTime >= 20000 && chickenHerd == nullptr) {
                    static uint32_t waitTime = 0;
                    if (waitTime == 0) waitTime = SDL_GetTicks();

                    if (SDL_GetTicks() - waitTime > 1000) {
                        currentState = STATE_WARNING;
                        warningStartTime = SDL_GetTicks();
                        waitTime = 0;
                    }
                }
            }
            // 5. Logic Boss
            else if (myBoss && myBoss->active) {
                updateBoss(myBoss, player->getY(), bossBullets);
                updateBossBullets();

                if (myBoss->health <= 0) {
                    currentState = STATE_WIN;
                    stateStartTime = SDL_GetTicks();
                    bossSpawned = false;
                    for (auto b : bossBullets) delete b;
                    bossBullets.clear();
                    return;
                }
            }
            handleCollisions();
        } break;

        case STATE_WARNING:
            player->update();
            if (SDL_GetTicks() - warningStartTime > 3000) {
                if (myBoss == nullptr) myBoss = createBoss(bossTex);
                bossSpawned = true;
                currentState = STATE_PLAYING;
            }
            break;

        case STATE_WIN:
        case STATE_GAMEOVER:
            if (SDL_GetTicks() - stateStartTime > 5000) {
                currentState = STATE_MENU;
                clearHerd(&chickenHerd);
                if (myBoss) { delete myBoss; myBoss = nullptr; }
                for (auto b : bossBullets) delete b;
                bossBullets.clear();
                bossSpawned = false;
            }
            break;
    }
}
void Game::updateBossBullets() {
    auto it = bossBullets.begin();
    while (it != bossBullets.end()) {
        (*it)->update();
        if (!(*it)->isActive()) {
            delete *it;
            it = bossBullets.erase(it);
        } else ++it;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_Rect rect1 = { 0, (int)bgY1, screenW, screenH };
    SDL_Rect rect2 = { 0, (int)bgY2, screenW, screenH };
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect1);
    SDL_RenderCopy(renderer, backgroundTex2, NULL, &rect2);

    if (currentState == STATE_MENU) {
        SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        SDL_RenderCopy(renderer, exitBtnTex, NULL, &btnExitRect);
    } else if (currentState == STATE_WIN || currentState == STATE_GAMEOVER) {
        SDL_Rect fullScreen = { 0, 0, screenW, screenH };
        SDL_RenderCopy(renderer, (currentState == STATE_WIN ? winTex : gameOverTex), NULL, &fullScreen);
    } else {
        if (bossSpawned && myBoss && myBoss->active) {
            renderBoss(renderer, myBoss);
            for (auto b : bossBullets) {
                SDL_Rect bRect = b->getRect();
                SDL_RenderCopy(renderer, bossBulletTex, NULL, &bRect);
            }
        }
        if (chickenHerd) renderHerd(chickenHerd, renderer, enemyTex);

        if (player) {
            for (auto b : player->bullets) {
                SDL_Rect bRect = b->getRect();
                SDL_RenderCopy(renderer, bulletTex, NULL, &bRect);
            }
            if (!player->isInvincible || (SDL_GetTicks() / 100) % 2 != 0) {
                SDL_Rect pRect = player->getRect();
                SDL_RenderCopy(renderer, playerTex, NULL, &pRect);
            }
        }

        for (int i = 0; i < playerLives; i++) {
            SDL_Rect lifeRect = { 10 + i * 35, 10, 30, 30 };
            SDL_RenderCopy(renderer, playerTex, NULL, &lifeRect);
        }

        if (currentState == STATE_WARNING) {
            SDL_Rect wRect = { 200, 250, 400, 100 };
            SDL_RenderCopy(renderer, warningTex, NULL, &wRect);
        }
    }
    SDL_RenderPresent(renderer);
}

void Game::handleCollisions() {
    if (!player || currentState != STATE_PLAYING) return;

    SDL_Rect pHitbox = player->getRect();
    pHitbox.x += 15; pHitbox.y += 15; pHitbox.w -= 30; pHitbox.h -= 30;

    // 1. Đạn người chơi bắn Boss/Gà
    for (auto b : player->bullets) {
        if (!b->isActive()) continue;
        SDL_Rect bRect = b->getRect();

        if (bossSpawned && myBoss && myBoss->active) {
            SDL_Rect bossRect = { (int)myBoss->x, (int)myBoss->y, myBoss->width, myBoss->height };
            if (Collision::check(bRect, bossRect)) {
                myBoss->health -= 10;
                b->destroy();
            }
        } else if (chickenHerd) {
            if (Collision::checkBulletHitHerd(&chickenHerd, bRect)) b->destroy();
        }
    }

    // 2. Kiểm tra người chơi bị trúng đòn
    if (!player->isInvincible) {
        bool hit = false;
        if (bossSpawned && myBoss && myBoss->active) {
            SDL_Rect bossRect = { (int)myBoss->x + 50, (int)myBoss->y + 50, myBoss->width - 100, myBoss->height - 100 };
            if (Collision::check(pHitbox, bossRect)) hit = true;

            // Dùng hàm check va chạm vector VIP
            if (Collision::checkPlayerWithBullets(pHitbox, bossBullets)) hit = true;
        } else if (Collision::checkPlayerHitHerd(chickenHerd, pHitbox)) hit = true;

        if (hit) {
            playerLives--;
            Mix_PlayChannel(-1, explodeSound, 0);
            if (playerLives <= 0) {
                currentState = STATE_GAMEOVER;
                stateStartTime = SDL_GetTicks();
            } else {
                player->makeInvincible();
                player->setX((screenW - player->getRect().w) / 2);
                player->setY(screenH - 150);
            }
        }
    }
}

void Game::clean() {
    if (shootSound) Mix_FreeChunk(shootSound);
    if (explodeSound) Mix_FreeChunk(explodeSound);
    for (auto b : bossBullets) delete b;
    bossBullets.clear();
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
