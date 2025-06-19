#include "Sprite.h"
#include <allegro5/allegro_image.h>
#include <iostream>

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
}

void Sprite::Init(float startX, float startY) {
    x = startX;
    y = startY;

    jumpImage = al_load_bitmap("jump.png");
    slashImage = al_load_bitmap("slash_oversize.png");
    slashReverseImage = al_load_bitmap("slash_reverse_oversize.png");
    hurtImage = al_load_bitmap("hurt.png");

    runImage = al_load_bitmap("run.png");

    ALLEGRO_BITMAP* combatIdleImage = al_load_bitmap("combat_idle.png");
    if (!combatIdleImage) {
        std::cerr << "Failed to load combat_idle.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(combatIdleImage, al_map_rgb(255, 0, 255));

    // Assuming it's a 3x3 grid with 8 usable frames (like run.png)
    std::vector<int> idleRows = { 1, 3 };
    combatIdleGrabber = new SpriteGrabber(combatIdleImage, frameWidth, frameHeight, animationColumns, 9, idleRows);

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
    facingRight = right;

    if (isMoving) {
        if (++frameCount >= frameDelay) {
            frameCount = 0;
            curFrame++;
            if (curFrame >= maxFrame)
                curFrame = 0;
        }
    }
    else {
        curFrame = 0;
    }
}

void Sprite::Draw(int xOffset, int yOffset) {
    ALLEGRO_BITMAP* frame = nullptr;

    if (isMoving && runGrabber) {
        frame = runGrabber->getFrame(curFrame);
    }
    else if (combatIdleGrabber) {
        frame = combatIdleGrabber->getFrame(curFrame);
    }


    if (frame) {
        if (facingRight) {
            // Flip to face right
            al_draw_bitmap(frame, x - xOffset, y - yOffset, ALLEGRO_FLIP_HORIZONTAL);
        }
        else {
            // No flip = face left (original sprite direction)
            al_draw_bitmap(frame, x - xOffset, y - yOffset, 0);
        }
    }
}





