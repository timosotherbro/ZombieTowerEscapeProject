#include "Sprite.h"
#include <allegro5/allegro_image.h>
#include <iostream>

Sprite::Sprite() {
    x = y = 0;
    isMoving = false;
    facingRight = true;

    frameWidth = 32;   
    frameHeight = 32;   
    curFrame = 0;
    maxFrame = 8;// Number of frames in run.png
    frameCount = 0;
    frameDelay = 6;
    animationColumns = 3;// 3 columns (based on 3x3 grid in run.png)

    idleImage = nullptr;
    runImage = nullptr;
    runGrabber = nullptr;

}

Sprite::~Sprite() {
    if (idleImage) al_destroy_bitmap(idleImage);
    if (runImage) al_destroy_bitmap(runImage);
    if (runGrabber) delete runGrabber;
}

void Sprite::Init(float startX, float startY) {
    x = startX;
    y = startY;

    idleImage = al_load_bitmap("idle.png");
    runImage = al_load_bitmap("run.png");

    if (!idleImage || !runImage) {
        std::cerr << "Failed to load sprite images!\n";
        exit(1);
    }

    if (!runImage) {
        std::cerr << "Failed to load run.png\n";
        exit(1);

        

    }



    // Convert magenta (255, 0, 255) to transparent
    al_convert_mask_to_alpha(idleImage, al_map_rgb(255, 0, 255));
    al_convert_mask_to_alpha(runImage, al_map_rgb(255, 0, 255));

    runGrabber = new SpriteGrabber(runImage, frameWidth, frameHeight, animationColumns, maxFrame);

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
    else {
        frame = idleImage;
    }

    if (frame) {
        if (facingRight) {
            al_draw_bitmap(frame, x - xOffset, y - yOffset, 0);
        }
        else {
            al_draw_bitmap(frame, x - xOffset, y - yOffset, ALLEGRO_FLIP_HORIZONTAL);
        }
    }
}




