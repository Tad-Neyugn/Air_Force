#include "Game.h"
#include "Collision.h"
#include <ctime>

Mix_Chunk* shootSound = nullptr;
Mix_Chunk* explodeSound = nullptr;

Game::Game() {
    isRunning = false;
    chickenHerd = nullptr;
    myBoss = nullptr;
    bossBullets = nullptr;
    currentState = STATE_MENU;
    playerLives = 3;
    btnPlayRect = { 300, 250, 200, 80 };
    btnExitRect = { 300, 350, 200, 80 };
    bossSpawned = false; // Đảm bảo khởi tạo cờ boss
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
        backgroundTex = IMG_LoadTexture(renderer, "assets/bg.png");
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
        bgY1 = 0; bgY2 = -600;
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
            if (currentState == STATE_MENU || currentState == STATE_GAMEOVER || currentState == STATE_WIN) {
                if (mx >= btnPlayRect.x && mx <= btnPlayRect.x + btnPlayRect.w &&
                    my >= btnPlayRect.y && my <= btnPlayRect.y + btnPlayRect.h) {
                    playerLives = 3;
                    bossSpawned = false;
                    myBoss = nullptr; // Reset boss khi chơi lại
                    clearHerd(&chickenHerd);
                    currentState = STATE_PLAYING;
                    gameStartTime = SDL_GetTicks();
                }
                if (mx >= btnExitRect.x && mx <= btnExitRect.x + btnExitRect.w &&
                    my >= btnExitRect.y && my <= btnExitRect.y + btnExitRect.h) {
                    isRunning = false;
                }
            }
        }
    }
    if (player && (currentState == STATE_PLAYING || currentState == STATE_WARNING))
        player->handleInput();
}

void Game::update() {
    bgY1 += scrollSpeed; bgY2 += scrollSpeed;
    if (bgY1 >= 600) bgY1 = bgY2 - 600;
    if (bgY2 >= 600) bgY2 = bgY1 - 600;

    switch (currentState) {
        case STATE_PLAYING: {
            player->update();

            // Nếu chưa đến lúc xuất hiện Boss
            if (!bossSpawned) {
                uint32_t elapsedTime = SDL_GetTicks() - gameStartTime;
                if (elapsedTime < 20000) {
                    static int spawnTimer = 0;
                    if (++spawnTimer >= 60) {
                        float randomX = (rand() % 700) + 50;
                        addEnemy(&chickenHerd, randomX, -50, rand() % 4, enemyTex);
                        spawnTimer = 0;
                    }
                    updateHerd(&chickenHerd, player);
                }
                else {
                    currentState = STATE_WARNING;
                    warningStartTime = SDL_GetTicks();
                    clearHerd(&chickenHerd);
                }
            }
            // Nếu Boss đã được tạo và đang active
            else if (myBoss && myBoss->active) {
                updateBoss(myBoss, player->getY(), &bossBullets, bossBulletTex);
                updateBossBullet(&bossBullets);

                // KHI BOSS CHẾT
                if (myBoss->health <= 0) {
                    currentState = STATE_WIN;
                    bossSpawned = false; // KHÓA: Ngăn lệnh vẽ Boss ngay lập tức

                    // Dọn dẹp sạch sẽ màn hình
                    clearHerd(&chickenHerd);
                    // Xóa đạn Boss còn sót lại
                    while(bossBullets) {
                        bossBulletNode* temp = bossBullets;
                        bossBullets = bossBullets->next;
                        delete temp->data;
                        delete temp;
                    }
                    return; // Dừng mọi logic update ngay lập tức để hiện ảnh thắng
                }
            }

            handleCollisions();
        } break;

        case STATE_WARNING: {
            player->update();
            if (SDL_GetTicks() - warningStartTime > 3000) {
                if (myBoss == nullptr) {
                    myBoss = createBoss(bossTex);
                }
                bossSpawned = true;
                currentState = STATE_PLAYING;
            }
        } break;

        default: break;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // 1. Vẽ nền (Background) luôn luôn vẽ đầu tiên
    SDL_Rect rect1 = { 0, (int)bgY1, 800, 600 };
    SDL_Rect rect2 = { 0, (int)bgY2, 800, 600 };
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect1);
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect2);

    if (currentState == STATE_MENU) {
        // Màn hình chính
        SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        SDL_RenderCopy(renderer, exitBtnTex, NULL, &btnExitRect);
    }
    else if (currentState == STATE_WIN) {
        // Ưu tiên vẽ ảnh Thắng tràn màn hình
        SDL_Rect fullScreen = { 0, 0, 800, 600 };
        SDL_RenderCopy(renderer, winTex, NULL, &fullScreen);
        SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        SDL_RenderCopy(renderer, exitBtnTex, NULL, &btnExitRect);
    }
    else if (currentState == STATE_GAMEOVER) {
        // Ưu tiên vẽ ảnh Thua tràn màn hình
        SDL_Rect fullScreen = { 0, 0, 800, 600 };
        SDL_RenderCopy(renderer, gameOverTex, NULL, &fullScreen);
        SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        SDL_RenderCopy(renderer, exitBtnTex, NULL, &btnExitRect);
    }
    else {
        // ĐANG CHƠI (CHỈ vẽ khi không ở trạng thái kết thúc)

        // 2. Vẽ Boss
        if (bossSpawned && myBoss && myBoss->active) {
            renderBoss(renderer, myBoss);
            renderBossBullet(renderer, bossBullets, bossBulletTex);
        }

        // 3. Vẽ Gà (Chicken Herd)
        if (chickenHerd) renderHerd(chickenHerd, renderer, enemyTex);

        // 4. Vẽ Player và Đạn Player
        if (player) {
            for (auto b : player->bullets) {
                SDL_Rect bRect = b->getRect();
                SDL_RenderCopy(renderer, bulletTex, NULL, &bRect);
            }
            SDL_Rect pRect = player->getRect();
            SDL_RenderCopy(renderer, playerTex, NULL, &pRect);
        }

        // 5. Vẽ UI Mạng (Chỉ vẽ khi đang chơi hoặc cảnh báo)
        if (currentState == STATE_PLAYING || currentState == STATE_WARNING) {
            for (int i = 0; i < playerLives; i++) {
                SDL_Rect lifeRect = { 10 + i * 35, 10, 30, 30 };
                SDL_RenderCopy(renderer, playerTex, NULL, &lifeRect);
            }
        }

        // 6. Hiệu ứng Cảnh báo Boss
        if (currentState == STATE_WARNING) {
            SDL_Rect wRect = { 200, 250, 400, 100 };
            SDL_RenderCopy(renderer, warningTex, NULL, &wRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::handleCollisions() {
    if (!player || currentState != STATE_PLAYING) return;

    SDL_Rect pRect = player->getRect();
    SDL_Rect pHitbox = { pRect.x + 20, pRect.y + 20, pRect.w - 40, pRect.h - 40 };

    // Player Bullets hit Enemy/Boss
    for (int i = 0; i < (int)player->bullets.size(); i++) {
        Bullet* b = player->bullets[i];
        if (!b->isActive()) continue;
        SDL_Rect bRect = b->getRect();
        bool hit = false;

        if (bossSpawned && myBoss && myBoss->active) {
            SDL_Rect bossHitbox = { (int)myBoss->x + 50, (int)myBoss->y + 20, myBoss->width - 100, myBoss->height - 100 };
            if (Collision::check(bRect, bossHitbox)) {
                takeDamage(myBoss, 10, &bossBullets);
                hit = true;
            }
        }
        else if (chickenHerd) {
            if (Collision::checkBulletHitHerd(&chickenHerd, bRect)) hit = true;
        }
        if (hit) b->destroy();
    }

    // Player Hit Enemy/Boss/BossBullets
    bool lostLife = false;

    if (bossSpawned && myBoss && myBoss->active) {
        SDL_Rect bBody = { (int)myBoss->x + 40, (int)myBoss->y + 40, myBoss->width - 80, myBoss->height - 80 };
        if (Collision::check(pHitbox, bBody)) lostLife = true;
        if (Collision::checkPlayerHitBossBullets(bossBullets, pHitbox)) lostLife = true;
    }
    else if (chickenHerd) {
        if (Collision::checkPlayerHitHerd(chickenHerd, pHitbox)) {
            lostLife = true;
        }
        else {
            EnemyNode* temp = chickenHerd;
            while(temp) {
                if(temp->data && temp->data->y > 580) {
                    lostLife = true;
                    break;
                }
                temp = temp->next;
            }
        }
    }

    if (lostLife) {
        playerLives--;
        Mix_PlayChannel(-1, explodeSound, 0);
        if (playerLives <= 0) {
            currentState = STATE_GAMEOVER;
        }
        else {
            clearHerd(&chickenHerd);
            player->setX(375);
            player->setY(500);
        }
    }
}

void Game::clean() {
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(explodeSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); SDL_Quit();
}
