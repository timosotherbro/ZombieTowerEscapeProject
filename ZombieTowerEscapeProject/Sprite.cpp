#include "Sprite.h"
#include <allegro5/allegro_image.h>
#include <iostream>
#include <string>
#include <sstream>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

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


    gravity = 0.5f;
    jumpStrength = -10.0f;
    yVelocity = 0;
    onGround = false;


}

Sprite::~Sprite() {
    if (combatIdleGrabber) delete combatIdleGrabber;
    if (runGrabber) delete runGrabber;
    if (runImage) al_destroy_bitmap(runImage);
    if (jumpImage) al_destroy_bitmap(jumpImage);
    if (slashImage) al_destroy_bitmap(slashImage);
    if (slashReverseImage) al_destroy_bitmap(slashReverseImage);
    if (hurtImage) al_destroy_bitmap(hurtImage);
    
    

    for (ALLEGRO_BITMAP* bmp : rightSlashFrames) al_destroy_bitmap(bmp);
    for (ALLEGRO_BITMAP* bmp : leftSlashFrames) al_destroy_bitmap(bmp);

    for (auto bmp : rightJumpFrames) al_destroy_bitmap(bmp);
    for (auto bmp : leftJumpFrames) al_destroy_bitmap(bmp);


}

void Sprite::Init(float startX, float startY) {
    x = startX;
    y = startY;

    
    hurtImage = al_load_bitmap("hurt.png");

    runImage = al_load_bitmap("run.png");

    // Load right idle frames
    for (int i = 1; i <= 2; ++i) {
        std::string filename = "rightidle" + std::to_string(i) + ".png";
        ALLEGRO_BITMAP* bmp = al_load_bitmap(filename.c_str());
        if (bmp) {
            al_convert_mask_to_alpha(bmp, al_map_rgb(255, 0, 255));
            rightIdleFrames.push_back(bmp);
        }
    }

    // Load left idle frames
    for (int i = 1; i <= 2; ++i) {
        std::string filename = "leftidle" + std::to_string(i) + ".png";
        ALLEGRO_BITMAP* bmp = al_load_bitmap(filename.c_str());
        if (bmp) {
            al_convert_mask_to_alpha(bmp, al_map_rgb(255, 0, 255));
            leftIdleFrames.push_back(bmp);
        }
    }

    for (int i = 0; i < 6; ++i) {
        std::stringstream ss;
        ss << "climb" << (i + 1) << ".png";
        climbFrames.push_back(al_load_bitmap(ss.str().c_str()));
    }




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







    // Load right jump frames
    for (int i = 1; i <= 5; i++) {
        std::stringstream ss;
        ss << "rightjump" << i << ".png";
        ALLEGRO_BITMAP* frame = al_load_bitmap(ss.str().c_str());
        if (frame) rightJumpFrames.push_back(frame);
    }

    // Load left jump frames
    for (int i = 1; i <= 5; i++) {
        std::stringstream ss;
        ss << "leftjump" << i << ".png";
        ALLEGRO_BITMAP* frame = al_load_bitmap(ss.str().c_str());
        if (frame) leftJumpFrames.push_back(frame);
    }


    

    

    




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

    if (isDead) {
        deathAngle += 0.1f;        // slowly spin
        deathScale -= 0.01f;       // slowly shrink
        if (deathScale < 0.1f) deathScale = 0.1f;  // clamp
        return; // skip normal updates when dead
    }

    
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

    if (onLadder) {
        if (!climbFrames.empty()) {
            state = "climb";
            animCounter++;
            if (animCounter >= 6) {
                animCounter = 0;
                currentFrame = (currentFrame + 1) % climbFrames.size();
            }
        }

        // Skip gravity while on ladder
        yVelocity = 0;
        return;
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
    else if (!isMoving && !isJumping) {
        currentMaxFrame = facingRight ? static_cast<int>(rightIdleFrames.size())
            : static_cast<int>(leftIdleFrames.size());
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


    // Apply gravity if not on ground
    if (!onGround) {
        yVelocity += gravity;
        if (yVelocity > maxFallSpeed) {
            yVelocity = maxFallSpeed;
        }
        y += yVelocity;
    }
    else {
        yVelocity = 0;
    }




}





void Sprite::Draw(int xOffset, int yOffset) {
    ALLEGRO_BITMAP* frame = nullptr;
    int flags = 0;  // by default no flipping


    if (state == "climb" && !climbFrames.empty()) {
        al_draw_bitmap(climbFrames[currentFrame], x - mapxoff, y - mapyoff, 0);
        return;
    }

    if (isAttacking) {
        if (facingRight && curFrame < rightSlashFrames.size()) {
            frame = rightSlashFrames[curFrame];
        }
        else if (!facingRight && curFrame < leftSlashFrames.size()) {
            frame = leftSlashFrames[curFrame];
        }
    }

    else if (isJumping) {
        std::vector<ALLEGRO_BITMAP*>& jumpFrames = facingRight ? rightJumpFrames : leftJumpFrames;
        if (!jumpFrames.empty()) {
            int jumpFrame = curFrame % jumpFrames.size();
            frame = jumpFrames[jumpFrame];
        }
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
    else if (isJumping) {
        std::vector<ALLEGRO_BITMAP*>& jumpFrames = facingRight ? rightJumpFrames : leftJumpFrames;
        int jumpFrame = (al_get_time() * 10);  // 10 FPS
        jumpFrame %= jumpFrames.size();
        if (jumpFrames[jumpFrame]) {
            al_draw_bitmap(jumpFrames[jumpFrame], x - xOffset, y - yOffset, 0);
        }
    }

    else if (!isMoving && !isJumping) {
        std::vector<ALLEGRO_BITMAP*>& idleFrames = facingRight ? rightIdleFrames : leftIdleFrames;
        if (!idleFrames.empty()) {
            int idleFrame = curFrame % idleFrames.size();
            frame = idleFrames[idleFrame];
        }
    }



    



    if (frame) {
        int centerX = x - xOffset + frameWidth / 2;
        int centerY = y - yOffset + frameHeight / 2;

        if (isDead) {
            al_draw_scaled_rotated_bitmap(
                frame,
                frameWidth / 2, frameHeight / 2, // center of rotation
                centerX, centerY,               // position on screen
                deathScale, deathScale,         // scale
                deathAngle,                     // rotation angle
                0                               // flags
            );
        }
        else {
            al_draw_bitmap(frame, centerX - frameWidth / 2, centerY - frameHeight / 2, flags);
        }
    }


    if (!onLadder && state == "climb") {
        state = "";  // Reset state when off ladder
        currentFrame = 0;
    }


}

void Sprite::triggerDeathAnimation() {
    isDead = true;
    deathAngle = 0.0f;
    deathScale = 1.0f;
}












