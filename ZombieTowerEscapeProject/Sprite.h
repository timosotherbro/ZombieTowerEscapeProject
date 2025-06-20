#pragma once
#include <allegro5/allegro.h>
#include "SpriteGrabber.h"

class Sprite {
public:
    Sprite();
    ~Sprite();

    void Init(float startX, float startY);
    void Update(bool isMoving, bool facingRight);
    void Draw(int xOffset, int yOffset);

    void setJumping(bool jumping) { 
        isJumping = jumping; 
    }

    void setAttacking(bool a) { 
        isAttacking = a;
    }



    float getX() const {
        return x; 
    }

    float getY() const {
        return y;
    }

    void setX(float newX) {
        x = newX; 
    }

    void setY(float newY) {
        y = newY; 
    }


    int getWidth() const {
        return frameWidth;
    }

    int getHeight() const {
        return frameHeight; 
    }


private:
    float x, y;
    bool isMoving;
    bool facingRight;
    bool isJumping;

    int frameWidth, frameHeight;
    int curFrame, maxFrame;
    int frameCount, frameDelay;
    int animationColumns;

    bool isAttacking;


    SpriteGrabber* combatIdleGrabber;
    SpriteGrabber* jumpGrabber;
    SpriteGrabber* slashGrabber;
    SpriteGrabber* slashReverseGrabber;

    ALLEGRO_BITMAP* runImage;
    ALLEGRO_BITMAP* jumpImage;
    ALLEGRO_BITMAP* slashImage;
    ALLEGRO_BITMAP* slashReverseImage;
    ALLEGRO_BITMAP* hurtImage;


    SpriteGrabber* runGrabber;
};
