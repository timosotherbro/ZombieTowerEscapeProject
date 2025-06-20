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
    if (slashGrabber) delete slashGrabber;
    if (slashReverseGrabber) delete slashReverseGrabber;

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
    slashImage = al_load_bitmap("slash_oversize.png");
    if (!slashImage) {
        std::cerr << "Failed to load slash_oversize.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(slashImage, al_map_rgb(255, 0, 255));

    // Slice row 2 (left-facing)
    std::vector<int> slashLeftRows = { 1 };
    slashGrabber = new SpriteGrabber(slashImage, frameWidth, frameHeight, 6, 24, slashLeftRows);

    // Slice row 4 (right-facing)
    std::vector<int> slashRightRows = { 3 };
    slashReverseGrabber = new SpriteGrabber(slashImage, frameWidth, frameHeight, 6, 24, slashRightRows);




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


    // Load and slice slash_oversize.png
    slashImage = al_load_bitmap("slash_oversize.png");
    if (!slashImage) {
        std::cerr << "Failed to load slash_oversize.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(slashImage, al_map_rgb(255, 0, 255));
    std::vector<int> slashRows = { 1, 3 };
    slashGrabber = new SpriteGrabber(slashImage, frameWidth, frameHeight, 5, 20, slashRows);

    // Load and slice slash_reverse_oversize.png
    slashReverseImage = al_load_bitmap("slash_reverse_oversize.png");
    if (!slashReverseImage) {
        std::cerr << "Failed to load slash_reverse_oversize.png!\n";
        exit(1);
    }
    al_convert_mask_to_alpha(slashReverseImage, al_map_rgb(255, 0, 255));
    slashReverseGrabber = new SpriteGrabber(slashReverseImage, frameWidth, frameHeight, 5, 20, slashRows);



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

    if (isAttacking && facingRight && slashGrabber) {
        currentMaxFrame = static_cast<int>(slashGrabber->getTotalFrames());
    }
    else if (isAttacking && !facingRight && slashReverseGrabber) {
        currentMaxFrame = static_cast<int>(slashReverseGrabber->getTotalFrames());
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

    if (isAttacking) {
        if (facingRight && slashGrabber) {
            frame = slashGrabber->getFrame(curFrame);
        }
        else if (!facingRight && slashReverseGrabber) {
            frame = slashReverseGrabber->getFrame(curFrame);
        }
    }
    else if (isJumping && jumpGrabber) {
        frame = jumpGrabber->getFrame(curFrame);
    }
    else if (isMoving && runGrabber) {
        frame = runGrabber->getFrame(curFrame);
    }
    else if (combatIdleGrabber) {
        frame = combatIdleGrabber->getFrame(curFrame);
    }


    if (frame) {
        bool needsFlip = lastDirectionRight;

        int flags = needsFlip ? ALLEGRO_FLIP_HORIZONTAL : 0;
        al_draw_bitmap(frame, x - xOffset, y - yOffset, flags);

    }
}










