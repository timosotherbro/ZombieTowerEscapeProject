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

    bool lastDirectionRight;

    bool isAttacking;

    float yVelocity = 0.0f;
    float gravity = 0.6f;
    float maxFallSpeed = 12.0f;
    bool onGround = false;


    std::vector<ALLEGRO_BITMAP*> rightSlashFrames;
    std::vector<ALLEGRO_BITMAP*> leftSlashFrames;


    SpriteGrabber* combatIdleGrabber;
    SpriteGrabber* jumpGrabber;
    
    



    ALLEGRO_BITMAP* runImage;
    ALLEGRO_BITMAP* jumpImage;
    ALLEGRO_BITMAP* slashImage;
    ALLEGRO_BITMAP* slashReverseImage;
    ALLEGRO_BITMAP* hurtImage;


    SpriteGrabber* runGrabber;
};
