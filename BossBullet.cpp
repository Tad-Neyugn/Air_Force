#include "BossBullet.h"

void addBossBullet(bossBulletNode** head, float x, float y, float sX, float sY, int w, int h, SDL_Texture* tex) {
    bossBullet* bullet = new bossBullet();
    bullet->x = x;
    bullet->y = y;
    bullet->speedX = sX;
    bullet->speedY = sY;
    bullet->width = w;
    bullet->height = h;
    bullet->active = true;
    bullet->texture = tex;

    bossBulletNode* newNode = new bossBulletNode();
    newNode->data = bullet;
    newNode->next = *head;
    *head = newNode;
}

void updateBossBullet(bossBulletNode** head) {
    bossBulletNode* curr = *head;
    bossBulletNode* prev = nullptr;

    while(curr != nullptr) {
        bossBullet* bullet = curr->data;
        if(bullet->active) {
            bullet->x += bullet->speedX;
            bullet->y += bullet->speedY;

            if(bullet->y > screenH || bullet->x < 0 || bullet->x > screenW) bullet->active = false;
        }

        if(!bullet->active) {
            bossBulletNode* temp = curr;
            if(prev == nullptr) *head = curr->next;
            else prev->next = curr->next;

            curr = curr->next;

            delete temp->data;
            delete temp;
        }

        else {
            prev = curr;
            curr = curr->next;
        }
    }
}
// BossBullet.cpp
void renderBossBullet(SDL_Renderer* renderer, bossBulletNode* head, SDL_Texture* tex) {
    bossBulletNode* curr = head;
    while (curr != nullptr) {
        bossBullet* bullet = curr->data;
        if (bullet != nullptr && bullet->active) {
            // Sử dụng kích thước từ struct bullet
            SDL_Rect rect = { (int)bullet->x, (int)bullet->y, bullet->width, bullet->height };

            if (tex != nullptr) {
                SDL_RenderCopy(renderer, tex, NULL, &rect);
            } else if (bullet->texture != nullptr) {
                SDL_RenderCopy(renderer, bullet->texture, NULL, &rect);
            } else {
                // Fallback nếu không có ảnh
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        curr = curr->next;
    }
}

void clearBossBullets(bossBulletNode** head) {
    bossBulletNode* curr = *head;
    while(curr != nullptr) {
        bossBulletNode* nextNode = curr->next;
        delete curr->data;
        delete curr;
        curr = nextNode;
    }
    *head = nullptr;
}

