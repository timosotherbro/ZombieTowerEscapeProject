#include "Sprite.h"
#include <allegro5/allegro_image.h>
#include <iostream>
#include <string>

Sprite::Sprite() {
    x = y = 0;
    isMoving = false;
    facingRight = true; 

    frameWidth = 64;   
    frameHeight = 64;   
    curFrame = 0;
    maxFrame = 8;// Number of frames in run.png
    frameCount = 0;
    frameDelay = 6;
    animationColumns = 3;// 3 columns (based on 3x3 grid in run.png)

    isJumping = false;
    isAttacking = false;

    lastDirectionRight = true;


    
    combatIdleGrabber = nullptr;
    runImage = nullptr;
    runGrabber = nullptr;

}

Sprite::~Sprite() {
    if (combatIdleGrabber) delete combatIdleGrabber;
    if (runGrabber) delete runGrabber;
    if (runImage) al_destroy_bitmap(runImage);
    if (jumpImage) al_destroy_bitmap(jumpImage);
    if (slashImage) al_destroy_bitmap(slashImage);
    if (slashReverseImage) al_destroy_bitmap(slashReverseImage);
    if (hurtImage) al_destroy_bitmap(hurtImage);
    if (jumpGrabber) delete jumpGrabber;
    

    for (ALLEGRO_BITMAP* bmp : rightSlashFrames) al_destroy_bitmap(bmp);
    for (ALLEGRO_BITMAP* bmp : leftSlashFrames) al_destroy_bitmap(bmp);

}

void Sprite::Init(float startX, float startY) {
    x = startX;
    y = startY;

    
    hurtImage = al_load_bitmap("hurt.png");

    runImage = al_load_bitmap("run.png");

    ALLEGRO_BITMAP* combatIdleImage = al_load_bitmap("combat_idle.png");
    if (!combatIdleImage) {
        std::cerr << "Failed to load combat_idle.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(combatIdleImage, al_map_rgb(255, 0, 255));

    std::vector<int> idleRows = { 1, 3 };  // rows 2 and 4
    combatIdleGrabber = new SpriteGrabber(combatIdleImage, frameWidth, frameHeight, 2, 8, idleRows);



    // Load slash image
   // Load right slash frames
    for (int i = 1; i <= 6; ++i) {
        std::string filename = "rightslash" + std::to_string(i) + ".png";
        ALLEGRO_BITMAP* bmp = al_load_bitmap(filename.c_str());
        if (!bmp) {
            std::cerr << "Failed to load " << filename << "\n";
            exit(1);
        }
        al_convert_mask_to_alpha(bmp, al_map_rgb(255, 0, 255));
        rightSlashFrames.push_back(bmp);
    }

    // Load left slash frames
    for (int i = 1; i <= 6; ++i) {
        std::string filename = "leftslash" + std::to_string(i) + ".png";
        ALLEGRO_BITMAP* bmp = al_load_bitmap(filename.c_str());
        if (!bmp) {
            std::cerr << "Failed to load " << filename << "\n";
            exit(1);
        }
        al_convert_mask_to_alpha(bmp, al_map_rgb(255, 0, 255));
        leftSlashFrames.push_back(bmp);
    }







    ALLEGRO_BITMAP* jumpSheet = al_load_bitmap("jump.png");
    if (!jumpSheet) {
        std::cerr << "Failed to load jump.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(jumpSheet, al_map_rgb(255, 0, 255));

    std::vector<int> jumpRows = { 1, 3 };  // rows 2 and 4
    jumpGrabber = new SpriteGrabber(jumpSheet, frameWidth, frameHeight, 5, 20, jumpRows);  // 5 columns, 20 frames

    al_destroy_bitmap(jumpSheet);  // only destroy after passing to grabber


    al_destroy_bitmap(combatIdleImage);

    if (!runImage) {
        std::cerr << "Failed to load run.png!\n";
        exit(1);
    }

    al_convert_mask_to_alpha(runImage, al_map_rgb(255, 0, 255));


    
    
    



    std::vector<int> allowedRows = { 1, 3 };
    runGrabber = new SpriteGrabber(runImage, frameWidth, frameHeight, animationColumns, 9, allowedRows);

    maxFrame = static_cast<int>(runGrabber->getTotalFrames());  // you'll add this method next
}



void Sprite::Update(bool moving, bool right) {
    isMoving = moving;

    
    if (moving) {
        facingRight = right;
        lastDirectionRight = right;
    }

    int currentMaxFrame = 0;

    if (isAttacking) {
        if (facingRight && !rightSlashFrames.empty())
            currentMaxFrame = static_cast<int>(rightSlashFrames.size());
        else if (!facingRight && !leftSlashFrames.empty())
            currentMaxFrame = static_cast<int>(leftSlashFrames.size());
    }


    else if (isJumping && jumpGrabber) {
        currentMaxFrame = static_cast<int>(jumpGrabber->getTotalFrames());
    }
    else if (isMoving && runGrabber) {
        currentMaxFrame = static_cast<int>(runGrabber->getTotalFrames());
    }
    else if (combatIdleGrabber) {
        currentMaxFrame = static_cast<int>(combatIdleGrabber->getTotalFrames());
    }


    if (++frameCount >= frameDelay) {
        frameCount = 0;
        curFrame++;
        if (curFrame >= currentMaxFrame) {
            curFrame = 0;
            if (isAttacking) {
                isAttacking = false;
            }
        }
    }
}





void Sprite::Draw(int xOffset, int yOffset) {
    ALLEGRO_BITMAP* frame = nullptr;
    int flags = 0;  // by default no flipping

    if (isAttacking) {
        if (facingRight && curFrame < rightSlashFrames.size()) {
            frame = rightSlashFrames[curFrame];
        }
        else if (!facingRight && curFrame < leftSlashFrames.size()) {
            frame = leftSlashFrames[curFrame];
        }
    }

    else if (isJumping && jumpGrabber) {
        frame = jumpGrabber->getFrame(curFrame);
    }
    else if (isMoving && runGrabber) {
        frame = runGrabber->getFrame(curFrame);
        if (facingRight) {
            flags = ALLEGRO_FLIP_HORIZONTAL;  // flip run animation only
        }
    }
    else if (combatIdleGrabber) {
        frame = combatIdleGrabber->getFrame(curFrame);
    }

    if (frame) {
        int drawX = x - xOffset + (frameWidth - al_get_bitmap_width(frame)) / 2;
        int drawY = y - yOffset + (frameHeight - al_get_bitmap_height(frame)) / 2;
        al_draw_bitmap(frame, drawX, drawY, flags);
    }

}













