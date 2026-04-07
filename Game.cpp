#include "Game.h"
#include "BasicData.h"
#include "HerdControl.h"
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
    // Khởi tạo hệ thống cốt lõi của SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "SDL khoi tao thanh cong!" << std::endl;

        // Tạo cửa sổ game
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, false);
        if (window) std::cout << "Tao cua so thanh cong!" << std::endl;

        // Tạo cây cọ vẽ (Renderer)
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set màu nền là Đen
            std::cout << "Tao renderer thanh cong!" << std::endl;
        }
        isRunning = true;

        gameStartTime = SDL_GetTicks();
        bossSpawned = false;
        myBoss = nullptr;
        bossBullets = nullptr;

        // Tạm thời để nullptr nếu chưa có file ảnh thực tế
        bossTex = nullptr;
        bossBulletTex = nullptr;

        // TẠO MÁY BAY: Đặt ở giữa cạnh dưới màn hình
        player = new Player(375, 500);

        // ---> THÊM: TẠO QUÁI VẬT ở giữa màn hình (X=400) nhưng trên cao ngoài tầm nhìn (Y=-50)
        chickenHerd = nullptr;
        srand(time(NULL));
    } else {
        isRunning = false;
    }

    backgroundTex1 = loadTexture("asset/bg1.bmp", renderer);
    backgroundTex2 = loadTexture("asset/bg2.bmp", renderer);
    bgY1 = 0;
    bgY2 = -screenH;
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
            // --------------------------

            default:
                break;
        }
    }

    // Giao việc bắt phím di chuyển cho Máy bay tự lo
    if (player) {
        player->handleInput();
    }
}

void Game::update() {
    // 1. Cập nhật Player (Luôn luôn cần thiết)
    if (player == nullptr) return;

    //if (player->getHealth() <= 0) {
        //isRunning = false;
        //return;
    //}
        player->update();

    // Lấy thời gian thực từ lúc mở Game
    uint32_t currentTime = SDL_GetTicks();
    uint32_t elapsedTime = currentTime - gameStartTime;

    // --- NHÁNH 1: TRƯỚC 2 PHÚT (Giai đoạn Gà) ---
    if (elapsedTime < BOSS_TRIGGER_TIME) {
        static int Timer = 0;
        Timer++;
        if(Timer >= 120) {
            float randomX = (rand() % 700) + 50;
            int type = rand() % 4;
            // Spawn gà như bình thường
            addEnemy(&chickenHerd, randomX, -50, type, nullptr);
            Timer = 0;
        }
        updateHerd(&chickenHerd, player);
    }
    // --- NHÁNH 2: ĐẾN GIỜ TRÙM CUỐI XUẤT HIỆN ---
    else if (!bossSpawned) {
        // Dọn sạch đàn gà đang bay lăng nhăng để Boss ra sân
        clearHerd(&chickenHerd);

        // Khởi tạo Boss (bossTex phải được load trong hàm init)
        myBoss = createBoss(bossTex);
        bossSpawned = true;

        std::cout << "WARNING: UNKNOWN AEROSPACE OBJECT DETECTED!" << std::endl;
    }
    // --- NHÁNH 3: CHIẾN ĐẤU VỚI BOSS ---
    else {
        if (myBoss && myBoss->active) {
            // Cập nhật logic di chuyển, phase và tấn công của Boss
            updateBoss(myBoss, player->getY(), &bossBullets, bossBulletTex);

            // Đừng quên cập nhật cả "mưa đạn" của Boss nhé
            updateBossBullet(&bossBullets);
        }
    }

    bgY1 += scrollSpeed;
    bgY2 += scrollSpeed;

    // Nếu tấm 1 trôi hết màn hình (600 là chiều cao màn hình)
    if (bgY1 >= 600) {
        bgY1 = bgY2 - 600;
    }
    // Nếu tấm 2 trôi hết màn hình
    if (bgY2 >= 600) {
        bgY2 = bgY1 - 600;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Vẽ 2 tấm nền tiếp sức
    SDL_Rect rect1 = { 0, (int)bgY1, 800, 600 };
    SDL_Rect rect2 = { 0, (int)bgY2, 800, 600 };

    // Tấm 1 dùng ảnh 1, Tấm 2 dùng ảnh 2
    SDL_RenderCopy(renderer, backgroundTex1, NULL, &rect1);
    SDL_RenderCopy(renderer, backgroundTex2, NULL, &rect2);

    // Sau đó mới vẽ các thứ khác
    if (player) player->render(renderer);
    if(chickenHerd != nullptr) renderHerd(chickenHerd, renderer);

    if (bossSpawned && myBoss != nullptr) {
        renderBoss(renderer, myBoss);
        renderBossBullet(renderer, bossBullets);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    clearHerd(&chickenHerd);
    // Dọn dẹp bộ nhớ trước khi tắt game để không bị nặng máy
    if (player) {
        delete player; // Hủy máy bay
    }

    // ---> THÊM: Hủy bộ nhớ của quái vật khi tắt game
    if (testEnemy) {
        delete testEnemy;
    }



    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
