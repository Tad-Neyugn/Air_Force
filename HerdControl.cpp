#include "HerdControl.h"
#include "Player.h"

void addEnemy(EnemyNode** head, float x, float y, int type, SDL_Texture* tex) {
    EnemyNode* newEnemyNode = new EnemyNode;
    newEnemyNode->data = createEnemy(x, y, type, enemyW, enemyH, tex);
    newEnemyNode->next = *head;
    *head = newEnemyNode;
}

void updateHerd(EnemyNode** head, Player* p) {
    EnemyNode* curr = *head;
    EnemyNode* prev = nullptr;

    while(curr != nullptr) {
        updateEnemy(curr->data, p);

        if(curr->data->active == false) {
            EnemyNode* toDelete = curr;

            if(prev == nullptr) *head = curr->next;
            else {
                prev->next = curr->next;
            }

            curr = curr->next;
            delete toDelete->data;
            delete toDelete;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

// HerdControl.cpp - Gom 2 hàm render làm 1
void renderHerd(EnemyNode* head, SDL_Renderer* renderer, SDL_Texture* tex) {
    EnemyNode* curr = head;
    while(curr != nullptr) {
        if (curr->data && curr->data->active) {
            SDL_Rect dest = { (int)curr->data->x, (int)curr->data->y, curr->data->width, curr->data->height };
            if (tex != nullptr) {
                SDL_RenderCopy(renderer, tex, NULL, &dest);
            } else {
                // Vẽ màu đỏ tạm thời nếu mất ảnh
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &dest);
            }
        }
        curr = curr->next;
    }
}

void clearHerd(EnemyNode** head) {
    EnemyNode* curr = *head;
    while(curr != nullptr) {
        EnemyNode* next = curr->next;
        delete curr->data;
        delete curr;
        curr = next;
    }
    *head = nullptr;
}


