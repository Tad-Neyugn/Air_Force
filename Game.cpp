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
        case STATE_PLAYING: { // Thêm ngoặc nhọn ở đây để fix lỗi "jump to case label"
            player->update();
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
            handleCollisions();
        } break;

        case STATE_WARNING:
            player->update();
            if (SDL_GetTicks() - warningStartTime > 3000) {
                myBoss = createBoss(bossTex);
                bossSpawned = true;
                currentState = STATE_PLAYING;
            }
            break;

        default: break; // Thêm default để hết warning
    }

    if (bossSpawned && myBoss && myBoss->active && currentState == STATE_PLAYING) {
        updateBoss(myBoss, player->getY(), &bossBullets, bossBulletTex);
        updateBossBullet(&bossBullets);
        if (myBoss->health <= 0) currentState = STATE_WIN;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_Rect rect1 = { 0, (int)bgY1, 800, 600 };
    SDL_Rect rect2 = { 0, (int)bgY2, 800, 600 };
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect1);
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect2);

    if (currentState == STATE_MENU) {
        SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        SDL_RenderCopy(renderer, exitBtnTex, NULL, &btnExitRect);
    } else {
        if (player) {
            for (auto b : player->bullets) {
                SDL_Rect bRect = b->getRect();
                SDL_RenderCopy(renderer, bulletTex, NULL, &bRect);
            }
            SDL_Rect pRect = player->getRect();
            SDL_RenderCopy(renderer, playerTex, NULL, &pRect);
        }
        if (chickenHerd) renderHerd(chickenHerd, renderer, enemyTex);
        if (bossSpawned && myBoss) {
            renderBoss(renderer, myBoss);
            renderBossBullet(renderer, bossBullets, bossBulletTex);
        }
        for (int i = 0; i < playerLives; i++) {
            SDL_Rect lifeRect = { 10 + i * 35, 10, 30, 30 };
            SDL_RenderCopy(renderer, playerTex, NULL, &lifeRect);
        }
        if (currentState == STATE_WARNING) {
            SDL_Rect wRect = { 200, 250, 400, 100 };
            SDL_RenderCopy(renderer, warningTex, NULL, &wRect);
        }
        if (currentState == STATE_GAMEOVER) {
            SDL_Rect goRect = { 200, 150, 400, 200 };
            SDL_RenderCopy(renderer, gameOverTex, NULL, &goRect);
            SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        }
        if (currentState == STATE_WIN) {
            SDL_Rect winR = { 200, 150, 400, 200 };
            SDL_RenderCopy(renderer, winTex, NULL, &winR);
            SDL_RenderCopy(renderer, playBtnTex, NULL, &btnPlayRect);
        }
    }
    SDL_RenderPresent(renderer);
}

void Game::handleCollisions() {
    if (!player || currentState != STATE_PLAYING) return;

    // 1. Cấu hình Hitbox của Player (Chỉnh lại độ nhạy để không quá khó chạm)
    SDL_Rect pRect = player->getRect();
    SDL_Rect pHitbox = { pRect.x + 20, pRect.y + 20, pRect.w - 40, pRect.h - 40 };

    // 2. Xử lý đạn của Player bắn trúng kẻ địch
    for (int i = 0; i < (int)player->bullets.size(); i++) {
        Bullet* b = player->bullets[i];
        if (!b->isActive()) continue;
        SDL_Rect bRect = b->getRect();
        bool hit = false;

        if (bossSpawned && myBoss && myBoss->active) {
            SDL_Rect bossHitbox = { (int)myBoss->x + 50, (int)myBoss->y + 20, myBoss->width - 60, myBoss->height - 100 };
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

    // 3. Kiểm tra va chạm gây mất mạng
    bool lostLife = false;

    if (bossSpawned && myBoss && myBoss->active) {
        // Va chạm với thân Boss hoặc đạn Boss
        SDL_Rect bBody = { (int)myBoss->x + 20, (int)myBoss->y + 30, myBoss->width - 40, myBoss->height - 60 };
        if (Collision::check(pHitbox, bBody)) lostLife = true;
        if (Collision::checkPlayerHitBossBullets(bossBullets, pHitbox)) lostLife = true;
    }
    else if (chickenHerd) {
        // Va chạm trực tiếp với gà
        if (Collision::checkPlayerHitHerd(chickenHerd, pHitbox)) {
            lostLife = true;
        }
        else {
            // KIỂM TRA GÀ RƠI XUỐNG ĐÁY MÀN HÌNH (600px)
            EnemyNode* temp = chickenHerd;
            while(temp) {
                // Chỉ cần 1 con vượt quá tọa độ y = 580 (gần chạm đáy) là tính mất mạng
                if(temp->data && temp->data->y > 580) {
                    lostLife = true;
                    break;
                }
                temp = temp->next;
            }
        }
    }

    // 4. Thực thi việc mất mạng và hồi sinh
    if (lostLife) {
        playerLives--;
        Mix_PlayChannel(-1, explodeSound, 0);

        if (playerLives <= 0) {
            currentState = STATE_GAMEOVER;
        }
        else {
            // Xóa sạch đàn gà cũ để không bị trừ mạng liên tục bởi con gà vừa rơi xuống
            clearHerd(&chickenHerd);

            // Đưa người chơi về vị trí an toàn (giữa màn hình phía dưới)
            player->setX(375);
            player->setY(500);

            // Có thể thêm hiệu ứng tạm thời không va chạm ở đây nếu muốn
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
