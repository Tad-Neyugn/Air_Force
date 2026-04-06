#include "Game.h"

Game::Game() {}
Game::~Game() {}

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

        // TẠO MÁY BAY: Đặt ở giữa cạnh dưới màn hình
        player = new Player(375, 500);

        // ---> THÊM: TẠO QUÁI VẬT ở giữa màn hình (X=400) nhưng trên cao ngoài tầm nhìn (Y=-50)
        testEnemy = new Enemy(400, -50);

    } else {
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT: // Khi người chơi bấm nút X tắt cửa sổ
            isRunning = false;
            break;
        default:
            break;
    }

    // Giao việc bắt phím di chuyển cho Máy bay tự lo
    if (player) {
        player->handleInput();
    }
}

void Game::update() {
    // Gọi Máy bay cập nhật tọa độ
    if (player) {
        player->update();
    }

    // ---> THÊM: Gọi Quái vật cập nhật tọa độ (rơi xuống)
    if (testEnemy && testEnemy->active) {
        testEnemy->update();
    }
}

void Game::render() {
    // BƯỚC 1: Xóa màn hình cũ đi (tô lại bằng màu đen)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // BƯỚC 2: Vẽ các vật thể lên (Vẽ máy bay)
    if (player) {
        player->render(renderer);
    }

    // ---> THÊM: Vẽ quái vật lên màn hình
    if (testEnemy && testEnemy->active) {
        testEnemy->render(renderer);
    }

    // BƯỚC 3: Đẩy bức tranh vừa vẽ xong ra màn hình cho người chơi xem
    SDL_RenderPresent(renderer);
}

void Game::clean() {
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
