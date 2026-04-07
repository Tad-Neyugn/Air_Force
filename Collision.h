#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>
#include "HerdControl.h" // Thêm thư viện chứa đàn gà
#include "Boss.h"

class Collision {
public:
    // Hàm 1: Check va chạm 2 hình chữ nhật cơ bản
    static bool check(const SDL_Rect& a, const SDL_Rect& b);

    // Hàm 2: Check đạn đâm trúng đàn gà (Cắt Node)
    static bool checkBulletHitHerd(EnemyNode** head, const SDL_Rect& bulletRect);

    // Hàm 3: Check máy bay đâm vào đàn gà
    static bool checkPlayerHitHerd(EnemyNode* head, const SDL_Rect& playerRect);

    //Hàm 4: Check đạn Boss bắn trúng máy bay
    static bool checkPlayerHitBossBullets(bossBulletNode* head, const SDL_Rect& playerRect);
};


#endif
